package bgu.spl.net.impl.stomp;

import java.nio.charset.StandardCharsets;

import bgu.spl.net.api.MessageEncoderDecoder;

public class FrameEncoderDecoder implements MessageEncoderDecoder<String>{

    private byte[] bytes = new byte[1 << 10]; //start with 1k
    private int len = 0;

    @Override
    public String decodeNextByte(byte nextByte){
        if (nextByte == '\n') {
             String ret = new String(bytes,StandardCharsets.UTF_8);
             for(int i=0;i<len;i++)
                bytes[i]='0';
            len=0;
             return ret;
        }
        if(len==bytes.length-1)
        {
            byte[] temp=new byte[bytes.length<<1];
            for(int i=0;i<bytes.length;i++)
                temp[i]=bytes[i];
            bytes=temp;
        }
        bytes[len]=nextByte;
        len++;
        return null; 
    }

    @Override
    public byte[] encode(String message){
        return message.getBytes();
    }
}
