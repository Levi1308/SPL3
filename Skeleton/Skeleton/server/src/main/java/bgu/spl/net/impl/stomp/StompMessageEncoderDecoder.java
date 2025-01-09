package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.MessageEncoderDecoder;

public class StompMessageEncoderDecoder implements MessageEncoderDecoder<String>{

    
    public String decodeNextByte(byte nextByte){
        return " ";

    }

   
    public byte[] encode(String message){
        return (message + "\n").getBytes(); //uses utf8 by default 

    }


}
