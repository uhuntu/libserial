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
    std::cout << "Writing input file contents to the serial port." << std::endl ;

    // Create a variable to store data from the input file and write to the
    // serial port.
    unsigned char data_byte[][17] = {
        {
            0x55,       // 1
            0xAA,       // 2
            0x82,       // 3
            11 * 1 + 1, // 4, length

            0x07,       // 5  = 11 * 1 - 6, down
            1 - 1,      // 6  = 11 * 1 - 5, Number
            0xFF,       // 7  = 11 * 1 - 4, X
            0x0F,       // 8  = 11 * 1 - 3
            0xFF,       // 9  = 11 * 1 - 2, Y
            0x0F,       // 10 = 11 * 1 - 1
            0xFF,       // 11 = 11 * 1 - 0, Z
            0x0F,       // 12 = 11 * 1 + 1
            0xB0,       // 13 = 11 * 1 + 2, byte 5
            0x00,       // 14 = 11 * 1 + 3
            0x00,       // 15 = 11 * 1 + 4

            1,          // n

            0x00        // checksum
        },
        {
            0x55,       // 1
            0xAA,       // 2
            0x82,       // 3
            11 * 1 + 1, // 4, length

            0x04,       // 5  = 11 * 1 - 6, up
            1 - 1,      // 6  = 11 * 1 - 5, Number
            0xFF,       // 7  = 11 * 1 - 4, X
            0x0F,       // 8  = 11 * 1 - 3
            0xFF,       // 9  = 11 * 1 - 2, Y
            0x0F,       // 10 = 11 * 1 - 1
            0xFF,       // 11 = 11 * 1 - 0, Z
            0x0F,       // 12 = 11 * 1 + 1
            0xB0,       // 13 = 11 * 1 + 2, byte 5
            0x00,       // 14 = 11 * 1 + 3
            0x00,       // 15 = 11 * 1 + 4

            1,          // n

            0x00        // checksum
        }
    };

    unsigned int sum = 0;

    for (unsigned int j = 0; j < (sizeof(data_byte) / sizeof(data_byte[0])); j ++) {

        sum = 0;

        for (unsigned int i = 0; i < sizeof(data_byte[0]); i ++) {

            if (i > 3) {
                sum += data_byte[j][i];
            }

            if (i == sizeof(data_byte[0]) - 1) {
                data_byte[j][i] = (unsigned char)(sum & 0x000000ff);
            }

            // Write the data to the serial port.
            serial_port.WriteByte(data_byte[j][i]) ;

            // Wait until the data has actually been transmitted.
            serial_port.DrainWriteBuffer() ;

            // Print to the terminal what is being written to the serial port.
    //        std::cout << data_byte ;
        }

    }

    // Successful program completion.
    std::cout << "The example program successfully completed!" << std::endl ;
    return EXIT_SUCCESS ;
}
