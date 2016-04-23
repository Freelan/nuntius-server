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
    sf::TcpListener listener;
    listener.listen(55001);
    std::cout << "Listening..." << std::endl;
    
    sf::TcpSocket socket;

    std::vector < sf::TcpSocket *> clients;
    sf::SocketSelector selector;
    selector.add( listener );

    std::vector<sf::IpAddress>addresses;

    while( true )
    {
        if( selector.wait( sf::seconds( 2 ) ) ) 
        {
            if( selector.isReady( listener ) )
            {
                sf::TcpSocket * tmp = new sf::TcpSocket;
                listener.accept( * tmp );
                clients.push_back( tmp );
                selector.add( * tmp );

                sf::IpAddress tmpIp = tmp->getRemoteAddress();
                addresses.push_back( tmpIp );

                std::cout << "New client connected: " << tmp->getRemoteAddress() << std::endl;
                //sendMessage( *tmp, tmpIp.toString() );
                sendMessage( *tmp, "Connected!" );
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
                    }else
                    {
                        std::cout << "[" << clients[i]->getRemoteAddress() << "] " << receivedMessage << std::endl;
                        for( int j = 0; j < clients.size(); j++ )
                        {
                            sendMessage( *clients[j], addresses[i].toString() );
                            sendMessage( *clients[j], receivedMessage );
                        }
                    }
                }
            }
            //...
        }
    }

    return 0;
}
