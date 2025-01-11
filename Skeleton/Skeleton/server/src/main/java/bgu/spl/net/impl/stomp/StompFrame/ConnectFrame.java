package bgu.spl.net.impl.stomp.StompFrame;

public class ConnectFrame extends StompFrame {
    private String version;
    private String host;
    private String login;
    private String passcode;

    public ConnectFrame(String version, String host, String login, String passcode) {
        super("CONNECT", null);
        this.version = version;
        this.host = "stomp.cs.bgu.ac.il";
        this.login = login;
        this.passcode = passcode;
    }

    // Getter for version
    public String getVersion() {
        return version;
    }

    // Getter for host
    public String getHost() {
        return host;
    }

    // Getter for login
    public String getLogin() {
        return login;
    }

    // Getter for passcode
    public String getPasscode() {
        return passcode;
    }
}