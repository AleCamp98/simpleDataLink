/**
 * @file simpleDataLink.h
 * @author Simone Bollattino (simone.bollattino@gmail.com)
 * @brief Simple data link protocol functions
 * 
 */

#ifndef SIMPLEDATALINK_H
#define SIMPLEDATALINK_H

#include "bufferUtils.h"
#include "frameUtils.h"

/**
 * @brief Macro which could be defined to put a limit on the frame payload
 * 
 * This can be useful to make sure that the used CRC is strong enough for the
 * maximum number of bytes sent.
 * This can be commented out to remove any limit to the frame size, otherwise
 * the functions will return error in case of frame too long.
 * 
 */
#define SDL_MAX_PAY_LEN 256

/**
 * @brief Struct containing transmission and reception functions of the serial
 *        line and reception buffer
 * 
 * This structure will be used as a handle function for the serial line, the
 * user should write TX and RX functions wrappers for the specific line used
 * the functions must be NON BLOCKING and have the following format:
 * 
 * byte argument: byte to be sent or pointer where to write received byte
 * return: 0 if the byte could not be sent, !0 otherwise
 * 
 * This approach makes the protocol higly portable and device independent.
 * 
 * The handle must be initialized with sdlInitLine(), this will assign the
 * function pointers and initialize the reception buffer, from that point
 * the user should never touch the handle members again but instead only use
 * sdlSend() and sdlReceive()
 */
typedef struct{
    uint8_t (*txFunc)(uint8_t byte); ///< TX function pointer
    uint8_t (*rxFunc)(uint8_t* byte); ///< RX function pointer
    circular_buffer_handle rxBuff;   ///< Rx buffer handle
    uint8_t rxBuffArray[SDL_MAX_PAY_LEN*2+6]; ///< Rx buffer memory array
}serial_line_handle;

/**
 * @brief Init serial line handle.
 * 
 * This function inits a serial line handle in order for it to be used with
 * sdlSend() and sdlReceive(), the function needs to receive the I/O functions
 * pointers as argument.
 * NB: txFunc and rxFunc can also be NULL if the serial line should work only
 * on TX or RX mode, in that case sdlSend() and sdlReceive() simply won't work.
 * See serial_line_handle documentation above for the format needed by those
 * functions.
 * 
 * @param line serial line handle to be initialized
 * @param txFunc tx function pointer 
 * @param rxFunc rx function pointer
 */
void sdlInitLine(serial_line_handle* line, uint8_t (*txFunc)(uint8_t byte), uint8_t (*rxFunc)(uint8_t* byte));

/**
 * @brief Send payload through serial line
 * 
 * This is the top function for transmission with this simple data link layer.
 * The function needs a serial line handler, a buffer containing the payload
 * and the lenght of the latter.
 * NB: The function will return error if the length len is higher than the
 * maximum allowed payload length, defined as the SDL_MAX_PAY_LEN macro.
 * 
 * @param line serial line handle where to send
 * @param buff array containing the payload
 * @param len length of the payload (must be <= SDL_MAX_PAY_LEN)
 * @return uint8_t 0 in case of error, !0 otherwise
 */
uint8_t sdlSend(serial_line_handle* line, uint8_t* buff, uint32_t len);

/**
 * @brief Receive payload from serial line
 * 
 * This is the top function for reception with this simple data link layer.
 * The function needs a serial line handler, a buffer where to write the
 * payload and the length of the latter.
 * NB: the len argument only represents the reception array dimension, the
 * actually received payload can be shorter and its length will be returned 
 * by the function. The len argument can have any value, also higher or lower
 * than SDL_MAX_PAY_LEN macro but it's recommended to at least provide a len
 * of SDL_MAX_PAY_LEN in order to not miss any payload.
 * The function will not return received payloads that are higher than
 * the len argument or SDL_MAX_PAY_LEN.
 * 
 * @param line serial line handle where to receive
 * @param buff array where the payload will be written
 * @param len length of the array
 * @return uint32_t length of the received payload, 0 if no payload or error
 */
uint32_t sdlReceive(serial_line_handle* line, uint8_t* buff, uint32_t len);

/**
 * @brief convert uin16_t to network order into buffer
 * 
 * Takes a 16 bit number and writes it inside a 2 elements buffer in
 * network order (big endian)
 * 
 * @param net destination buffer (MUST BE LONG 2!)
 * @param num source number
 */
void num16ToNet(uint8_t net[2], uint16_t num);

//convert network order from buffer into uint16_t
//NB: net MUST BE AT LEAST LONG 2!!
/**
 * @brief convert buffer containing network order number to uint16_t
 * 
 * Takes a buffer containing a 2 byte number in network order (big endian) and
 * returns a 16 bit variable in host order (big or little endian)
 * 
 * @param net source buffer (MUST BE LONG 2!)
 * @return uint16_t destination number
 */
uint16_t netToNum16(uint8_t net[2]);

#endif