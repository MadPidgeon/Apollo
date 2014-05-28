#pragma once
#include <deque>
#include <vector>
#include <iostream>
#include <cstdint>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/function.hpp>

using boost::asio::ip::tcp;
using namespace std;

typedef std::deque<chat_message> chat_message_queue;

class chat_client {
public:
    chat_client(boost::asio::io_service& io_service,
            tcp::resolver::iterator endpoint_iterator)
        : io_service_(io_service),
            socket_(io_service),
            my_channel_(1)
    {
        boost::asio::async_connect(socket_, endpoint_iterator,
                boost::bind(&chat_client::handle_connect, this,
                    boost::asio::placeholders::error));
    }
    void write( const chat_message& );
    void write( uint8_t, uint8_t, uint8_t );
    void close();
    void set_message_reactor( boost::function<void(vector<uint8_t>*)> );
    uint8_t my_channel() const;
    void request_channel( uint8_t );
private:
    void handle_connect( const boost::system::error_code& );
    void handle_read_header( const boost::system::error_code& );
    void handle_read_body( const boost::system::error_code& );
    void do_write( chat_message );
    void handle_write( const boost::system::error_code& error );
    void do_close();
    
private:
    boost::asio::io_service& io_service_;
    tcp::socket socket_;
    chat_message read_msg_;
    chat_message_queue write_msgs_;
    boost::function<void(vector<uint8_t>*)> messageReactor;
    uint8_t my_channel_;
};