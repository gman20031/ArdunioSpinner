
// https://learn.microsoft.com/en-us/dotnet/api/system.io.ports.serialport?view=net-11.0-pp#definition 

using System.IO.Ports;
using System.Runtime.InteropServices;

public class Listener
{
    public enum MessageId
    {
        mouseMovement,
        disconnect,
        connected,
        attachedInterrupts,
        testConncetion,
        failure,
        count,
    };

    [StructLayout (LayoutKind.Sequential, Pack = 1)]
    public class Message
    {
        const int Size = 3;
        public MessageId ID { get; set; }
        public int dX { get; set; } = 0;
        public int dY { get; set; } = 0;
 
        public void Read( SerialPort port )
        {
            byte[] buffer = new byte[Size];

            // read whole message
            int bytesReceived = 0;
            while ( bytesReceived < Size )
                bytesReceived += port.Read( buffer , bytesReceived , Size - bytesReceived);

            // convert buffer to data.
            if ( buffer[0] >= (int)MessageId.count )
                Console.WriteLine( $"ERROR message id was {buffer[0]}" );

            ID = (MessageId)buffer[0];
            dX = (sbyte)buffer[1];
            dY = (sbyte)buffer[2];
        }

        public string Text { 
            get {
                switch ( ID )
                {
                    case MessageId.mouseMovement: return $"Movement{{{dX},{dY}}}";
                    case MessageId.disconnect: return "disconnected";
                    case MessageId.connected: return "connected";
                    case MessageId.attachedInterrupts: return "interrupts Attached";
                    case MessageId.testConncetion: return "test";
                    case MessageId.failure: return "Failure";
                    default: return string.Empty;
                }
            }
        } 
    }

    private Thread m_readThread;
    public SerialPort Port { get; private set; }

    public bool Active { get; set; } = false;
    public string Name { get; set; } = "invalid";
     
    public Listener( SerialPort port, string name )
    {
        Port = port;
        Active = true;
        Name = name;
        port.Open();
        m_readThread = new( this.Read );

        if (!port.IsOpen)
        {
            Console.WriteLine( $"Port is closed, {port.PortName}" );
            return;
        }

        m_readThread.Start();
    }

    ~Listener()
    {
        m_readThread.Join(); // im sure this is implicit, but why not.
    }

    public void Read()
    {
        Message msg = new();
        while(Active)
        {
            try
            {
                msg.Read( Port );
                
                Console.WriteLine( msg.Text );

                if ( msg.ID == MessageId.disconnect )
                    Active = false;
            }
            catch ( TimeoutException ) { }
        }
    }
}


internal class Program
{

    public static void Main( string[] args )
    {
        Console.WriteLine( "Started" );

        SerialPort port = PortFinder.FindPort();

        Listener listener = new( port , "UNO3" );
        if ( ! listener.Active )
            return;

        StringComparer stringComparer = StringComparer.OrdinalIgnoreCase;
        Console.WriteLine( "Write QUIT to exit" );

        while (listener.Active)
        {
            string line = Console.ReadLine() ?? string.Empty;

            if ( stringComparer.Equals( "quit" , line ) )
                listener.Active = false;
        }
    }

}
