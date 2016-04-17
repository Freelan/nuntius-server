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
    // Wait for a connection
    sf::TcpSocket socket;


    std::vector < sf::TcpSocket *> clients; // tutaj przechowujemy klientów
    sf::SocketSelector selector; // selektor
    selector.add( listener ); // dodajemy gniazdo nasłuchujące

    while( true )
    {
        if( selector.wait( sf::seconds( 2 ) ) ) // jeśli metoda wait() zwróci true, to znaczy, że któreś z dodanych gniazd jest gotowe do odbioru
        {                                          // jako argument podajemy czas, przez który ma czekać na dane
            if( selector.isReady( listener ) ) // metoda isReady() sprawdza, czy dane gniazdo ma dane do odebrania
            // jeśli do motedy isReady() przekażemy gniazdo nasłuchujące, true oznacza, że ktoś chce się do niego podłaczyć
            {
                sf::TcpSocket * tmp = new sf::TcpSocket;
                listener.accept( * tmp ); // skoro ktoś chce się do nas połączyć, to go akceptujemy
                clients.push_back( tmp ); // i dodajemy go do listy
                selector.add( * tmp ); // oraz do selektora, żeby można było od niego odbierać dane

                std::cout << "New client connected: " << tmp->getRemoteAddress() << std::endl;
                sendMessage( *tmp, "Connected!" );

                // nie zapomnij, by usunąć(za pomocą delete) gniazdo, kiedy się rozłączy
            }

            // pętla przechodząca po kontenerze gniazd (zależy od typu kontenera)
            for( int i = 0; i < clients.size(); i++ )
            {
                if( selector.isReady( * clients[ i ] ) ) // *clients[i] coś nam wysłał
                {
                    std::string killMessage = "kill";
                    std::string receivedMessage = receiveMessage( *clients[ i ] );

                    if( receivedMessage == killMessage || receivedMessage == "whoops!" )
                    {
                        std::cout << clients[ i ]->getRemoteAddress() << " disconnected!" << std::endl;
                        selector.remove( *clients[i] );
                    }else
                    {
                        std::cout << "[" << clients[ i ]->getRemoteAddress() << "] " << receivedMessage << std::endl;

                        for( int i = 0; i < clients.size(); i++ )
                        {
                            sendMessage( *clients[ i ], receivedMessage );
                        }
                    }
                    // tutaj robimy coś z odebranymi dany
                    //...
                }
            }
            //...
            // reszta kodu serwera
        }
    }

    return 0;
}
