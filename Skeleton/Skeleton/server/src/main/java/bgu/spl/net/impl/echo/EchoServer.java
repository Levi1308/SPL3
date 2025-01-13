package bgu.spl.net.impl.echo;

import bgu.spl.net.srv.Server;
import bgu.spl.net.impl.echo.EchoProtocol;
import bgu.spl.net.impl.echo.LineMessageEncoderDecoder;
import bgu.spl.net.impl.stomp.StompProtocolImpl;

public class EchoServer {

    public static void main(String[] args) {

        // you can use any server... 
        
        Server.threadPerClient(
                7777, //port
                () -> new StompProtocolImpl(), //protocol factory
                LineMessageEncoderDecoder::new //message encoder decoder factory
        ).serve();
        
        // Server.reactor(
        //         Runtime.getRuntime().availableProcessors(),
        //         7777, //port
        //         () -> new EchoProtocol<>(), //protocol factory
        //         LineMessageEncoderDecoder::new //message encoder decoder factory
        // ).serve();
    }
}
