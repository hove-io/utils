#include "utils/zmq.h"
#include "utils/exception.h"
#include <array>

void z_send(zmq::socket_t& socket, const std::string& str, zmq::send_flags flags) {
    zmq::message_t msg(str.size());
    std::memcpy(msg.data(), str.c_str(), str.size());
    socket.send(msg, flags);
}

void z_send(zmq::socket_t& socket, zmq::message_t& msg, zmq::send_flags flags) {
    socket.send(msg, flags);
}

std::string z_recv(zmq::socket_t& socket) {
    zmq::message_t msg;
    socket.recv(msg, zmq::recv_flags::none);
    return std::string(static_cast<char*>(msg.data()), msg.size());
}


LoadBalancer::LoadBalancer(zmq::context_t& context) : clients(context, ZMQ_ROUTER), workers(context, ZMQ_ROUTER) {}
void LoadBalancer::bind(const std::string& clients_socket_path, const std::string& workers_socket_path) {
    clients.bind(clients_socket_path.c_str());
    workers.bind(workers_socket_path.c_str());
}

void LoadBalancer::run() {

    std::vector<std::string> frames {};
    while (true) {
        zmq_pollitem_t items[] = {{static_cast<void*>(workers), 0, ZMQ_POLLIN, 0},
                                  {static_cast<void*>(clients), 0, ZMQ_POLLIN, 0}};
        if (available_workers.empty()) {
            // we don't look for request from client if there is no worker for handling them
            zmq::poll(items, 1, -1);
        } else {
            zmq::poll(items, 2, -1);
        }
        // handle worker
        if (items[0].revents & ZMQ_POLLIN) {

            // the first frame is the identifier of the worker: we add it to the available workers list
            available_workers.push(z_recv(workers));

            // the second frame should be empty
            std::string empty = z_recv(workers);
            if (empty != "") {
                // just skip the rest of the message
                continue;
            }

            frames.clear();
            size_t more = 0;
            size_t more_size = sizeof (more);
            do {
                std::string frame = z_recv(workers);
                frames.push_back(frame);
                // Are there more frames coming?
                workers.getsockopt(ZMQ_RCVMORE, &more, &more_size);
            } while (more);


            if (frames.size() < 3) {
                continue;
            }
            if (frames[0] == "READY") {
                // the worker just signaled it is ready, nothing to do
                continue;
            }

            //here we should get a response from the worker

            // send every remaining frames to the client
            for (size_t idx = 0; idx < frames.size() - 1; ++ idx) {
                z_send(clients, frames[idx], zmq::send_flags::sndmore);
            }
            z_send(clients, frames.back());

        }
        // handle clients request
        if (items[1].revents & ZMQ_POLLIN){

            // The client request is a multi-part ZMQ message, we have to check every frame and be sure the multi-part message frame
            // is composed as we wish,

            // The multi-part ZMQ message should have :
            //  - one or more frames identifying the client
            //  - then an empty frame
            //  - and finally one frame with the actual request payload
            size_t more = 0;
            size_t more_size = sizeof (more);

            frames.clear();

            do {
                std::string frame = z_recv(clients);
                frames.push_back(frame);

                // Are there more frames coming?
                clients.getsockopt(ZMQ_RCVMORE, &more, &more_size);
            } while (more);


            // if we have less than 3 frames, the message is ill-formed, and we ignore it
            if (frames.size() < 3) {
                continue;
            }
            // if we the penultimate frame is not empty, the message is ill-formed, and we ignore it
            if (frames[frames.size() - 2] != "") {
                continue;
            }

            std::string worker_addr = available_workers.top();
            available_workers.pop();

            // let's forward the message to the workers
            
            // a first frame identifying the worker to route the request to
            z_send(workers, worker_addr, zmq::send_flags::sndmore);
            // then an empty frame
            z_send(workers, "", zmq::send_flags::sndmore);

            // and then the message from the client
            for (size_t idx = 0; idx < frames.size() - 1; ++ idx) {
                z_send(workers, frames[idx], zmq::send_flags::sndmore);
            }
            z_send(workers, frames.back());
        }
    }
}
