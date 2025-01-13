package bgu.spl.net.impl.stomp;

import bgu.spl.net.impl.rci.ObjectEncoderDecoder;
import bgu.spl.net.impl.rci.RemoteCommandInvocationProtocol;
import bgu.spl.net.srv.Server;

public class StompServer {

    public static void main(String[] args) {
        Server.reactor(
                 Runtime.getRuntime().availableProcessors(),
                 7777, //port
                 () -> new StompProtocolImpl(), //protocol factory
                 FrameEncoderDecoder::new //message encoder decoder factory
         ).serve();

         Server.threadPerClient(
                7777, //port
                () -> new StompProtocolImpl(), //protocol factory
                FrameEncoderDecoder::new //message encoder decoder factory
        ).serve();
       
    }
}
