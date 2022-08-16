#include "Session.hpp"

#define BOOST_NO_EXCEPTIONS
#include <boost/throw_exception.hpp>
void boost::throw_exception(std::exception const& e)
{
    // do nothing
}

constexpr auto print_frame = [](auto& frame) {
    for (auto e : frame) {
        std::cout << std::hex << static_cast<int>(e) << " ";
    }
    std::cout << "\n";
};

class server
{
  public:
    server(boost::asio::io_context& io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
    {
        do_accept();
    }
    void loop()
    {
        for (size_t i = 0; i < mSessionList.size(); i++) {
            auto v = mSessionList[i]->readFrame();
            if (v.has_value()) {
                std::cout << "Hello2\n";

                print_frame(v.value());
                mSessionList[i]->writeFrame(v.value());
            }
        }
    }

  private:
    std::vector<std::shared_ptr<Session>> mSessionList;
    void do_accept()
    {
        acceptor_.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
            if (!ec) {
                std::cout << "Hello\n";
                auto ss = std::make_shared<Session>(std::move(socket));
                ss->start();
                mSessionList.push_back(ss);
            }
            do_accept();
        });
    }

    tcp::acceptor acceptor_;
};

int main()
{
    boost::asio::io_context io_context;
    server s(io_context, 8888);
    std::thread ctxThread([&io_context]() {
        io_context.run();
    });

    while (true) {
        s.loop();
    }

    ctxThread.join();
    return 0;
}
