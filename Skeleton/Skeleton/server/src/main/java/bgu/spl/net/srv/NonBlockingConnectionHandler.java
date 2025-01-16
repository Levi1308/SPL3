package bgu.spl.net.srv;

import bgu.spl.net.api.MessageEncoderDecoder;
import bgu.spl.net.api.StompMessagingProtocol;

import java.util.LinkedList;
import java.util.List;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.nio.ByteBuffer;
import java.util.Queue;
import java.nio.channels.SocketChannel;
import java.io.IOException;
import java.nio.channels.SelectionKey;

public class NonBlockingConnectionHandler<T> implements ConnectionHandler<T> {
    private static final int BUFFER_ALLOCATION_SIZE = 1 << 13; //8k
    private static final ConcurrentLinkedQueue<ByteBuffer> BUFFER_POOL = new ConcurrentLinkedQueue<>();

    private final StompMessagingProtocol<T> protocol;
    private final MessageEncoderDecoder<T> encdec;
    private final Queue<ByteBuffer> writeQueue = new ConcurrentLinkedQueue<>();
    private final SocketChannel chan;
    private final Reactor<T> reactor;

    private List<String> channelsSubscribed;

    private int connectionID;
    private Connections<T> connections;

    public NonBlockingConnectionHandler(
            Connections<T> connection,
            int ID,
            MessageEncoderDecoder<T> reader,
            StompMessagingProtocol<T> protocol,
            SocketChannel chan,
            Reactor<T> reactor) {
        connections = connection;
        this.chan = chan;
        this.encdec = reader;
        this.protocol = protocol;
        this.reactor = reactor;
        channelsSubscribed = new LinkedList<>();
        connectionID = ID;
        connections.addConnection(connectionID, this);
    }

    public Runnable continueRead() {
        ByteBuffer buf = leaseBuffer();

        boolean success = false;
        try {
            success = chan.read(buf) != -1;
        } catch (IOException ex) {
            System.out.println(ex.getMessage());
        }

        if (success) {
            buf.flip();
            // Creating a task, Notice that this code is very similar to the BlockingConnectionHandler's main loop
            return () -> {
                try {
                    while (buf.hasRemaining()) {
                        T nextMessage = encdec.decodeNextByte(buf.get());
                        if (nextMessage != null) {
                            protocol.start(connectionID, connections);
                            protocol.process(nextMessage);
                        }
                    }
                } finally {
                    releaseBuffer(buf);
                }
            };
        } else {
            releaseBuffer(buf);
            close();
            return null;
        }

    }

    public void close() {
        try {
            chan.close();
        } catch (IOException ex) {
            System.out.println(ex.getMessage());
        }
    }

    public void continueWrite() {
        while (!writeQueue.isEmpty() && chan.isOpen()) {
            try {
                ByteBuffer top = writeQueue.peek();
                chan.write(top);
                if (top.hasRemaining()) {
                    return;
                } else {
                    writeQueue.remove();
                }
            } catch (IOException ex) {
                System.out.println(ex.getMessage());
                close();
            }
        }

        if (writeQueue.isEmpty()) {
            if (protocol.shouldTerminate()) close();
            else reactor.updateInterestedOps(chan, SelectionKey.OP_READ);
        }
    }

    private static ByteBuffer leaseBuffer() {
        ByteBuffer buff = BUFFER_POOL.poll();
        if (buff == null) {
            return ByteBuffer.allocateDirect(BUFFER_ALLOCATION_SIZE);
        }

        buff.clear();
        return buff;
    }

    private static void releaseBuffer(ByteBuffer buff) {
        BUFFER_POOL.add(buff);
    }

    @Override
    public void send(T msg) {
        //IMPLEMENT IF NEEDED
        writeQueue.add(ByteBuffer.wrap(encdec.encode(msg)));
        reactor.updateInterestedOps(chan, SelectionKey.OP_READ | SelectionKey.OP_WRITE);
    }

    public List<String> getChannelsSubscribed() {
        return channelsSubscribed;
    }
}
