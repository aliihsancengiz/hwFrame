#pragma once

#include "Framer.hpp"

#include <boost/asio.hpp>
#include <queue>

using boost::asio::ip::tcp;
class Session : public std::enable_shared_from_this<Session>, public framer::Framer
{
    using frameReceivedCallback = std::function<void(frameType)>;

  public:
    Session(boost::asio::ip::tcp::socket socket, frameReceivedCallback cb = nullptr)
        : socket_(std::move(socket)), mReceivedCb(cb)
    {
    }

    void start()
    {
        receiveFrame();
    }
    void writeFrame(frameType f)
    {
        pushData(f, framer::PushType::ENCODER);
    }
    std::optional<frameType> readFrame()
    {
        if (mReceivedFrameQueue.size() > 0) {
            auto rcv = mReceivedFrameQueue.front();
            mReceivedFrameQueue.pop();
            return rcv;
        }
        return std::nullopt;
    }

  private:
    void receiveFrame()
    {
        auto self(shared_from_this());
        socket_.async_read_some(boost::asio::buffer(rxBuffer, 1024),
                                [this, self](boost::system::error_code ec, std::size_t length) {
                                    if (!ec) {
                                        std::vector<uint8_t> ff(rxBuffer, rxBuffer + length);
                                        pushData(ff, framer::PushType::DECODER);
                                        receiveFrame();
                                    }
                                });
    }

    boost::asio::ip::tcp::socket socket_;
    char rxBuffer[1024];
    std::queue<frameType> mReceivedFrameQueue;
    frameReceivedCallback mReceivedCb;

  protected:
    void onDecodedFrame(frame_events::GotDecodedFrame frame) override
    {
        if (mReceivedCb) {
            mReceivedCb(frame.data);
        }
        mReceivedFrameQueue.emplace(frame.data);
    }
    void onEncodedFrame(frame_events::GotEncodedFrame frame) override
    {
        auto self(shared_from_this());
        boost::asio::async_write(
          socket_, boost::asio::buffer(frame.data.data(), frame.data.size()),
          [this, self](boost::system::error_code ec, std::size_t /*length*/) {});
    }
};
