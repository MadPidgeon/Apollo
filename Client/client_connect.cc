#include <cstdlib>
#include <deque>
#include <vector>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/function.hpp>
#include "./../Global/chat_message.hpp"
#include "client_connect.h"

using boost::asio::ip::tcp;
using namespace std;

void chat_client::write(const chat_message& msg) {
    io_service_.post(boost::bind(&chat_client::do_write, this, msg));
}

void chat_client::write( uint8_t a, uint8_t b, uint8_t c ) {
    chat_message msg;
    msg.body_length(3);
    msg.body()[0] = a;
    msg.body()[1] = b;
    msg.body()[2] = c;
    msg.encode_header();
    io_service_.post( boost::bind(&chat_client::do_write, this, msg) );
}

void chat_client::close() {
    io_service_.post(boost::bind(&chat_client::do_close, this));
}

void chat_client::handle_connect(const boost::system::error_code& error) {
    if (!error) {
        boost::asio::async_read(socket_,
            boost::asio::buffer(read_msg_.data(), chat_message::header_length),
            boost::bind(&chat_client::handle_read_header, this,
                boost::asio::placeholders::error));
    }
}

void chat_client::handle_read_header(const boost::system::error_code& error) {
    if (!error && read_msg_.decode_header()) {
        boost::asio::async_read(socket_,
            boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
            boost::bind(&chat_client::handle_read_body, this,
                boost::asio::placeholders::error));
    } else {
        do_close();
    }
}

void chat_client::handle_read_body(const boost::system::error_code& error) {
    if (!error) {
        //std::cout.write(read_msg_.body(), read_msg_.body_length());
        //std::cout << "\n";
        if( read_msg_.body_length() == 3 ) {
            //cout << (int) read_msg_.body()[0] << "," << (int) read_msg_.body()[1] << "," << (int) read_msg_.body()[2] << endl;
            vector<uint8_t> mmes( read_msg_.body(), read_msg_.body()+3 );
            boost::thread r( boost::bind( messageReactor,  &mmes ) );
        } else {
            cout.write(read_msg_.body(), read_msg_.body_length());
            std::cout << "\n";
        }

        boost::asio::async_read(socket_,
                boost::asio::buffer(read_msg_.data(), chat_message::header_length),
                boost::bind(&chat_client::handle_read_header, this,
                    boost::asio::placeholders::error));
    } else {
        do_close();
    }
}

void chat_client::do_write(chat_message msg) {
    bool write_in_progress = !write_msgs_.empty();
    write_msgs_.push_back(msg);
    if (!write_in_progress) {
        boost::asio::async_write(socket_,
            boost::asio::buffer(write_msgs_.front().data(),
                write_msgs_.front().length()),
            boost::bind(&chat_client::handle_write, this,
                boost::asio::placeholders::error));
    }
}

void chat_client::handle_write(const boost::system::error_code& error) {
    if (!error) {
        write_msgs_.pop_front();
        if (!write_msgs_.empty()) {
            boost::asio::async_write(socket_,
                    boost::asio::buffer(write_msgs_.front().data(),
                        write_msgs_.front().length()),
                    boost::bind(&chat_client::handle_write, this,
                        boost::asio::placeholders::error));
        }
    } else {
        do_close();
    }
}

void chat_client::do_close() {
    socket_.close();
}

void chat_client::set_message_reactor( boost::function<void(vector<uint8_t>*)> mr ) {
    messageReactor = mr;
}

uint8_t chat_client::my_channel() const {
    return my_channel_;
}

void chat_client::request_channel( uint8_t c ) {
    /* TEMP */
    my_channel_ = c;
}