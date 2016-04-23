#include <iostream>
#include <SFML/Network.hpp>

void sendMessage( sf::TcpSocket& socket, std::string message )
{
    sf::Packet packet;
    packet << message;
    socket.send( packet );
}

std::string receiveMessage( sf::TcpSocket& socket )
{
    sf::Packet packet;
    std::string message;
    if( socket.receive( packet ) == sf::Socket::Done )
    {
        packet >> message;
        return message;
    }else
    {
        return "whoops!";
    }
}

int main()
{
    // ----- The server -----
    sf::TcpSocket socket;
    sf::TcpListener listener;
    listener.listen(55001);
    std::cout << "Listening..." << std::endl;

    std::vector<sf::IpAddress>addresses;
    std::vector < sf::TcpSocket *> clients;
    sf::SocketSelector selector;
    selector.add( listener );

    while( true )
    {
        if( selector.wait( sf::seconds( 2 ) ) )
        {
            if( selector.isReady( listener ) )
            {
                sf::TcpSocket * tmp = new sf::TcpSocket;
                if( listener.accept( * tmp )  == sf::Socket::Done )
                {
                    clients.push_back( tmp );
                    selector.add( * tmp );

                    sf::IpAddress tmpIp = tmp->getRemoteAddress();
                    addresses.push_back( tmpIp );

                    std::cout << "New client connected: " << tmp->getRemoteAddress() << std::endl;
                    sendMessage( *tmp, "\r-- CONNECTED --" );
                }else
                {
                    delete tmp;
                }
            }

            for( int i = 0; i < clients.size(); i++ )
            {
                if( selector.isReady( *clients[i] ) )
                {
                    std::string killMessage = "kill";
                    std::string receivedMessage = receiveMessage( *clients[i] );

                    if( receivedMessage == killMessage || receivedMessage == "whoops!" )
                    {
                        std::cout << clients[i]->getRemoteAddress() << " disconnected!" << std::endl;
                        selector.remove( *clients[i] );
                        clients[i]->disconnect();
                        clients.erase( clients.begin() + i );
                        addresses.erase( addresses.begin() + i );
                    }else
                    {
                        std::cout << "[" << clients[i]->getRemoteAddress() << "] " << receivedMessage << std::endl;

                        for( int j = 0; j < clients.size(); j++ )
                        {
                            sf::Packet packet;
                            packet << addresses[i].toString() << receivedMessage;
                            clients[j]->send( packet );

                            //std::cout << "Send " << addresses[i].toString() << " und " << receivedMessage << " to client " << j << std::endl;
                        }
                    }
                }
            }
            //...
        }
    }

    return 0;
}
