package bgu.spl.net.impl.newsfeed;

import bgu.spl.net.impl.rci.Command;
import java.io.Serializable;
import bgu.spl.net.impl.newsfeed.NewsFeed;

public class PublishNewsCommand implements Command<NewsFeed> {
 
    private String channel;
    private String news;
 
    public PublishNewsCommand(String channel, String news) {
        this.channel = channel;
        this.news = news;
    }


    @Override
    public Serializable execute(NewsFeed arg) {
        return null;
    }
}