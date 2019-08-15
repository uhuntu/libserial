/**
 *  @example serial_port_write.cpp
 */

#include <libserial/SerialPort.h>

#include <cstdlib>
#include <fstream>
#include <iostream>

constexpr const char* const SERIAL_PORT_2 = "/dev/ttyUSB0" ;

/**
 * @brief This example reads the contents of a file and writes the entire 
 *        file to the serial port one character at a time. To use this
 *        example, simply utilize TestFile.txt or another file of your
 *        choosing as a command line argument.
 */
int main(int argc, char** argv)
{   
    using namespace LibSerial ;
    // Determine if an appropriate number of arguments has been provided.
    if (argc < 2)
    {
        // Error message to the user.
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl ;

        // Exit the program if no input file argument has been given.
        return 1 ;
    }

    // Open the input file for reading. 
    std::ifstream input_file(argv[1]) ;

    // Determine if the input file argument is valid to read data from.
    if (!input_file.good()) 
    {
        std::cerr << "Error: Could not open file "
                  << argv[1] << " for reading." << std::endl ;
        return 1 ;
    }

    // Instantiate a SerialPort object.
    SerialPort serial_port ;

    try
    {
        // Open the Serial Port at the desired hardware port.
        serial_port.Open(SERIAL_PORT_2) ;
    }
    catch (const OpenFailed&)
    {
        std::cerr << "The serial port did not open correctly." << std::endl ;
        return EXIT_FAILURE ;
    }

    // Set the baud rate of the serial port.
    serial_port.SetBaudRate(BaudRate::BAUD_115200) ;

    // Set the number of data bits.
    serial_port.SetCharacterSize(CharacterSize::CHAR_SIZE_8) ;

    // Turn off hardware flow control.
    serial_port.SetFlowControl(FlowControl::FLOW_CONTROL_NONE) ;

    // Disable parity.
    serial_port.SetParity(Parity::PARITY_NONE) ;
    
    // Set the number of stop bits.
    serial_port.SetStopBits(StopBits::STOP_BITS_1) ;

    // Read characters from the input file and write them to the serial port. 
//    std::cout << "Writing input file contents to the serial port." << std::endl ;

    unsigned char data_bytes_x82[17];

    unsigned int sum = 0;

    while (input_file.peek() != EOF) 
    {
        // Create a variable to store data from the input file and write to the
        // serial port.
        char data_bytes_x81[16] ;

        sum = 0;

        // Read data from the input file.
        input_file.read(data_bytes_x81, 16) ;

        data_bytes_x82[0] = 0x55;               // 0x55
        data_bytes_x82[1] = 0xaa;               // 0xaa
        data_bytes_x82[2] = 0x82;               // 0x82
        data_bytes_x82[3] = 0x0c;               // length

        data_bytes_x82[4] = data_bytes_x81[4];  // down or up

        data_bytes_x82[5] = 0x00;               // n - 1

        data_bytes_x82[6] = data_bytes_x81[6];  // x l
        data_bytes_x82[7] = data_bytes_x81[7];  // x h
        data_bytes_x82[8] = data_bytes_x81[8];  // y l
        data_bytes_x82[9] = data_bytes_x81[9];  // y h

        data_bytes_x82[10] = 0x00;              // z l
        data_bytes_x82[11] = 0x00;              // z h
        data_bytes_x82[12] = 0xb0;              // byte 5
        data_bytes_x82[13] = 0x00;              // x tilt
        data_bytes_x82[14] = 0x00;              // y tilt
        data_bytes_x82[15] = 0x01;              // n
        data_bytes_x82[16] = 0x00;              // checksum

        for (unsigned char c : data_bytes_x82) {
            sum += c;
        }

        // re-sum
        data_bytes_x82[16] = (unsigned char)(sum & 0x000000ff);

        for (unsigned char c : data_bytes_x82) {
            // Write the data to the serial port.
            serial_port.WriteByte(c) ;

            // Wait until the data has actually been transmitted.
            serial_port.DrainWriteBuffer() ;

            // Print to the terminal what is being written to the serial port.
            //std::cout << c;
        }
    }

    input_file.close();

    // Successful program completion.
//    std::cout << "The example program successfully completed!" << std::endl ;

    return EXIT_SUCCESS ;
}
