package bgu.spl.net.impl.stomp;

public class ConnectFrame extends StompFrame {
    private String version;
    private String host;
    private String login;
    private String passcode;

    public ConnectFrame(String version, String host, String login, String passcode, String body) {
        super("CONNECT");
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
