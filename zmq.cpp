#include "utils/zmq.h"

void z_send(zmq::socket_t& socket, const std::string& str, int flags) {
    zmq::message_t msg(str.size());
    std::memcpy(msg.data(), str.c_str(), str.size());
    socket.send(msg, flags);
}

void z_send(zmq::socket_t& socket, zmq::message_t& msg, int flags) {
    socket.send(msg, flags);
}

std::string z_recv(zmq::socket_t& socket) {
    zmq::message_t msg;
    socket.recv(&msg);
    return std::string(static_cast<char*>(msg.data()), msg.size());
}

namespace {

bool is_valid_message(const std::vector<zmq::message_t>& frames) {
    return frames.size() == 3 && frames[1].size() == 0;
}

}  // end namespace

LoadBalancer::LoadBalancer(zmq::context_t& context) : clients(context, ZMQ_ROUTER), workers(context, ZMQ_ROUTER) {}
void LoadBalancer::bind(const std::string& clients_socket_path, const std::string& workers_socket_path) {
    clients.bind(clients_socket_path.c_str());
    workers.bind(workers_socket_path.c_str());
}

void LoadBalancer::run() {
    while (true) {
        zmq_pollitem_t items[] = {{static_cast<void*>(workers), 0, ZMQ_POLLIN, 0},
                                  {static_cast<void*>(clients), 0, ZMQ_POLLIN, 0}};
        if (avalailable_worker.empty()) {
            // we don't look for request from client if there is no worker for handling them
            zmq::poll(items, 1, -1);
        } else {
            zmq::poll(items, 2, -1);
        }
        // handle worker
        if (items[0].revents & ZMQ_POLLIN) {
            // the first frame is the identifier of the worker: we add it to the available worker
            avalailable_worker.push(z_recv(workers));
            {
                //  Second frame is empty
                std::string empty = z_recv(workers);
                assert(empty.size() == 0);
            }

            //  Third frame is READY or else a client reply address
            std::string client_addr = z_recv(workers);

            //  If client reply, send resp back to the appropriate client
            if (client_addr != "READY") {
                {
                    // another empty frame
                    std::string empty = z_recv(workers);
                    assert(empty.size() == 0);
                }
                // the actual reply
                zmq::message_t reply;
                workers.recv(&reply);
                z_send(clients, client_addr, ZMQ_SNDMORE);
                z_send(clients, "", ZMQ_SNDMORE);
                z_send(clients, reply);
            }
        }
        // handle clients request
        if (items[1].revents & ZMQ_POLLIN) {
            // The client request is a multi-part ZMQ message, we have to check every frame and be sure the multi-part
            // message frame is composed as we wish, otherwise the multi-part message may be shifted unexpectedly.

            // The multi-part ZMQ message should have 3 parts
            // The first one is the ID of message
            // The second one is an empty frame
            // The third one is the real request
            int more = 0;
            size_t more_size = sizeof(more);

            std::vector<zmq::message_t> frames{};
            do {
                zmq::message_t frame;
                clients.recv(&frame);
                frames.push_back(std::move(frame));
                // Are there more frames coming?
                clients.getsockopt(ZMQ_RCVMORE, &more, &more_size);
            } while (more);

            if (!is_valid_message(frames)) {
                z_send(clients, "");
                continue;
            }

            std::string worker_addr = avalailable_worker.top();
            avalailable_worker.pop();

            z_send(workers, worker_addr, ZMQ_SNDMORE);
            z_send(workers, "", ZMQ_SNDMORE);
            // frames[0] is the id of message
            z_send(workers, frames[0], ZMQ_SNDMORE);
            z_send(workers, "", ZMQ_SNDMORE);
            // frames[2] is the request
            z_send(workers, frames[2]);
        }
    }
}
