package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.MessageEncoderDecoder;

public class FrameEncoderDecoder implements MessageEncoderDecoder<String>{

    private byte[] bytes = new byte[1 << 10]; //start with 1k
    private int len = 0;

    @Override
    public String decodeNextByte(byte nextByte){
        return " ";

    }

    @Override
    public byte[] encode(String message){
        return (message + "\n").getBytes(); //uses utf8 by default 

    }

    private void pushByte(byte nextByte) {}

    private String popString(){return " ";}


}
