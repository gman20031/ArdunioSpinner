using System;
using System.Collections.Generic;
using System.Text;

using System.IO.Ports;

internal class PortFinder
{
    const int kTimeout = 500;

    public static SerialPort FindPort(bool verbose = false)
    {
        SerialPort port = new()
        {
            PortName = SetPortName(),
            ReadTimeout = kTimeout ,
            WriteTimeout = kTimeout,
            DtrEnable = true,
            RtsEnable = true,
        };

        if(verbose)
        {
            port.BaudRate  = SetPortBaudRate( port.BaudRate );
            port.Parity    = SetPortParity( port.Parity );
            port.DataBits  = SetPortDataBits( port.DataBits );
            port.StopBits  = SetPortStopBits( port.StopBits );
            port.Handshake = SetPortHandshake( port.Handshake );
        }

        return port;
    }
    
    // Display Port values and prompt user to enter a port.
    private static string SetPortName()
    {
        Console.WriteLine( "Available Ports:" );
        string[] portNames =  SerialPort.GetPortNames();
        if ( portNames.Length <= 0 )
            return string.Empty;
        foreach ( string name in portNames )
        {
            Console.WriteLine( "   {0}" , name );
        }

        Console.Write( "Enter COM port value (Default: {0}): " , portNames[0] );
        string portName = Console.ReadLine() ?? portNames[0];

        if ( portName.IsWhiteSpace() || !portName.StartsWith( "com" , StringComparison.CurrentCultureIgnoreCase ) )
            portName = portNames[0];
        return portName;
    }
    // Display BaudRate values and prompt user to enter a value.
    private static int SetPortBaudRate( int defaultPortBaudRate )
    {
        Console.Write( "Baud Rate(default:{0}): " , defaultPortBaudRate );
        string baudRate = Console.ReadLine() ?? defaultPortBaudRate.ToString();
        return baudRate.IsWhiteSpace() ? defaultPortBaudRate : int.Parse( baudRate );
    }

    // Display PortParity values and prompt user to enter a value.
    private static Parity SetPortParity( Parity defaultPortParity )
    {
        Console.WriteLine( "Available Parity options:" );
        foreach ( string s in Enum.GetNames<Parity>() )
        {
            Console.WriteLine( "   {0}" , s );
        }

        Console.Write( "Enter Parity value (Default: {0}):" , defaultPortParity.ToString() );

        string parity = Console.ReadLine() ?? defaultPortParity.ToString();
        return parity.IsWhiteSpace() ? defaultPortParity : Enum.Parse<Parity>( parity , true );
    }
    // Display DataBits values and prompt user to enter a value.
    private static int SetPortDataBits( int defaultPortDataBits )
    {
        Console.Write( "Enter DataBits value (Default: {0}): " , defaultPortDataBits );
        string dataBits = Console.ReadLine() ?? defaultPortDataBits.ToString();
        return dataBits.IsWhiteSpace() ? defaultPortDataBits : int.Parse( dataBits.ToUpperInvariant() );
    }

    // Display StopBits values and prompt user to enter a value.
    private static StopBits SetPortStopBits( StopBits defaultPortStopBits )
    {
        Console.WriteLine( "Available StopBits options:" );
        foreach ( string s in Enum.GetNames<StopBits>() )
        {
            Console.WriteLine( "   {0}" , s );
        }

        Console.Write( "Enter StopBits value (None is not supported and \n" +
            "raises an ArgumentOutOfRangeException. \n (Default: {0}):" , defaultPortStopBits.ToString() );
        string stopBits = Console.ReadLine() ?? defaultPortStopBits.ToString();
        return stopBits.IsWhiteSpace() ? defaultPortStopBits : Enum.Parse<StopBits>( stopBits , true );
    }

    private static Handshake SetPortHandshake( Handshake defaultPortHandshake )
    {
        Console.WriteLine( "Available Handshake options:" );
        foreach ( string s in Enum.GetNames<Handshake>() )
        {
            Console.WriteLine( "   {0}" , s );
        }

        Console.Write( "Enter Handshake value (Default: {0}):" , defaultPortHandshake.ToString() );
        string handshake = Console.ReadLine() ?? defaultPortHandshake.ToString();
        return handshake.IsWhiteSpace() ? defaultPortHandshake : Enum.Parse<Handshake>( handshake , true );
    }


}

