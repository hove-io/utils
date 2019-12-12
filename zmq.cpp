#include "utils/zmq.h"


void z_send(zmq::socket_t& socket, const std::string& str, int flags){
    zmq::message_t msg(str.size());
    std::memcpy(msg.data(), str.c_str(), str.size());
    socket.send(msg, flags);
}

void z_send(zmq::socket_t& socket, zmq::message_t& msg, int flags){
    socket.send(msg, flags);
}

std::string z_recv(zmq::socket_t& socket){
    zmq::message_t msg;
    socket.recv(&msg);
    return std::string(static_cast<char*>(msg.data()), msg.size());
}

namespace {

void purge_and_reply(zmq::socket_t& clients, int more, bool should_purge) {

    size_t more_size = sizeof (more);
	while(more && should_purge) {
		z_recv(clients);
		clients.getsockopt(ZMQ_RCVMORE, &more, &more_size);
	}
	z_send(clients, "");
}

} // end namespace

LoadBalancer::LoadBalancer(zmq::context_t& context):
            clients(context, ZMQ_ROUTER), workers(context, ZMQ_ROUTER){
}
void LoadBalancer::bind(const std::string& clients_socket_path, const std::string& workers_socket_path){
    clients.bind(clients_socket_path.c_str());
    workers.bind(workers_socket_path.c_str());
}

void LoadBalancer::run(){
    while(true){
        zmq_pollitem_t items [] = {
            {static_cast<void*>(workers), 0, ZMQ_POLLIN, 0},
            {static_cast<void*>(clients), 0, ZMQ_POLLIN, 0}
        };
        if(avalailable_worker.empty()){
            //we don't look for request from client if there is no worker for handling them
            zmq::poll(items, 1, -1);
        }else{
            zmq::poll(items, 2, -1);
        }
        //handle worker
        if (items[0].revents & ZMQ_POLLIN) {
            //the first frame is the identifier of the worker: we add it to the available worker
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
                    //another empty frame
                    std::string empty = z_recv(workers);
                    assert(empty.size() == 0);
                }
                //the actual reply
                zmq::message_t reply;
                workers.recv(&reply);
                z_send(clients, client_addr, ZMQ_SNDMORE);
                z_send(clients, "", ZMQ_SNDMORE);
                z_send(clients, reply);
            }
        }
        //handle clients request
        if (items[1].revents & ZMQ_POLLIN){
            // The client request is a multi-part ZMQ message, we have to check every frame and be sure the multi-part message frame
            // is composed as we wish, otherwise the multi-part message may be shifted unexpectedly.

            int more;
            size_t more_size = sizeof (more);

            //first we get the client id
            zmq::message_t msg_identity;
            clients.recv(&msg_identity);
            // Are there more frames coming?
            clients.getsockopt(ZMQ_RCVMORE, &more, &more_size);
            assert(more == 1);

            // We're waiting for an empty frame...
            // If no more frames, we reply immediately
            if (more == 0) {
                bool should_purge = false;
                purge_and_reply(clients, more, should_purge);
                continue;
            }

            //then an empty frame, there should be another frame follows up...
            std::string empty = z_recv(clients);
            clients.getsockopt(ZMQ_RCVMORE, &more, &more_size);
            assert(empty.empty());
            assert(more == 1);

            if (!empty.empty() || more == 0){
                // Case 1: If no more frames follow up, we reply and return
                // Case 2: the current frame is NOT empty, so we consider it as a bad request, we purge the queue and then reply
                bool should_purge = true;
                purge_and_reply(clients, more, should_purge);
                continue;
            }

            zmq::message_t msg_request;
            clients.recv(&msg_request);
            clients.getsockopt(ZMQ_RCVMORE, &more, &more_size);
            assert(more == 0);

            if (more != 0) {
                // the current frame must be the last frame, if there are more frames coming, we consider it as a bad request
                // we purge the queue and then reply same as what's done above
                bool should_purge = true;
                purge_and_reply(clients, more, should_purge);
                continue;
            }
            std::string worker_addr = avalailable_worker.top();
            avalailable_worker.pop();

            z_send(workers, worker_addr, ZMQ_SNDMORE);
            z_send(workers, "", ZMQ_SNDMORE);
            z_send(workers, msg_identity, ZMQ_SNDMORE);
            z_send(workers, "", ZMQ_SNDMORE);
            z_send(workers, msg_request);
        }
    }
}

