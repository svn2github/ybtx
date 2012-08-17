/*
 * @(#)thinkgear.h    4.1    Jun 26, 2009
 *
 * Copyright (c) 2008-2009 NeuroSky, Inc. All Rights Reserved
 * NEUROSKY PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 */

/* Ensure this header is only included once */
#ifndef THINKGEAR_H_
#define THINKGEAR_H_

/**
 * @file thinkgear.h
 *
 * This header file defines the ThinkGear Communications Driver (TGCD) API,
 * which is a set of functions for users to connect to and receive data
 * from a ThinkGear data stream.
 *
 * There are three "tiers" of functions in this API, where functions
 * of a lower "tier" should not be called until an appropriate function
 * in the "tier" above it is called first:
 *
 * @code
 *
 * Tier 1:  Call anytime
 *     TG_GetDriverVersion()
 *     TG_GetNewConnectionId()
 *
 * Tier 2:  Requires TG_GetNewConnectionId()
 *     TG_SetStreamLog()
 *     TG_SetDataLog()
 *     TG_Connect()
 *     TG_FreeConnection()
 *
 * Tier 3:  Requires TG_Connect()
 *     TG_ReadPackets()
 *     TG_GetValue()
 *     TG_SendByte()
 *     TG_SetBaudrate()
 *     TG_SetDataFormat()
 *     TG_Disconnect()
 *
 * @endcode
 *
 * Regarding preprocessor macro constants:  Typical users do not need to
 * (and should NOT) define any special macros in order to use this header
 * file with a ThinkGear Communications Driver shared library (i.e. .dll
 * or .so). The following macros should only be defined by maintainers of
 * the ThinkGear Communications Driver shared library itself:
 *     THINKGEAR_DLL_COMPILE (or NO_DLL_MODIFIERS)
 *     COMPILE_FOR_JNI
 *
 * @author Kelvin Soo
 * @version 4.1 Jun 26, 2009 Kelvin Soo
 *   - Updated name of library from ThinkGear Connection API to
 *     ThinkGear Communications Driver (TGCD). The library still
 *     uses ThinkGear Connection objects and IDs to communicate.
 * @version 4.0 May 14, 2008 Kelvin Soo
 *   - Overhauled API entirely to support new features of ThinkGear-EM.
 * @version 3.2 Nov 29, 2007 Kelvin Soo
 *   - Initial version.
 */

/* Disable name-mangling when compiling as C++ */
#ifdef __cplusplus
extern "C" {
#endif

/* This block is standard for DLL macro definitions, and
 * typical users should ignore this block entirely.
 * Maintainers of the DLL should define THINKGEAR_DLL_COMPILE
 * when compiling the DLL.
 */
#ifdef NO_DLL_MODIFIERS
#define THINKGEAR_API
#else
#if defined(_WIN32)
#if defined(THINKGEAR_DLL_COMPILE)
#define THINKGEAR_API __declspec( dllexport )
#else /* !THINKGEAR_DLL_COMPILE */
#define THINKGEAR_API __declspec( dllimport )
#endif /* !THINKGEAR_DLL_COMPILE */
#else /* !_WIN32 */
#define THINKGEAR_API extern
#endif /* !_WIN32 */
#endif /* !NO_DLL_MODIFIERS */
/* Allow only for XP and above */
#if !defined(WINVER)
#define WINVER 0x0501
#endif

/**
 * Maximum number of Connections that can be requested before being
 * required to free one.
 */
#define TG_MAX_CONNECTION_HANDLES 128

/**
 * Baud rate for use with TG_Connect() and TG_SetBaudrate().
 */
#define TG_BAUD_1200         1200
#define TG_BAUD_2400         2400
#define TG_BAUD_4800         4800
#define TG_BAUD_9600         9600
#define TG_BAUD_57600       57600
#define TG_BAUD_115200     115200

/**
 * Data format for use with TG_Connect() and TG_SetDataFormat().
 */
#define TG_STREAM_PACKETS      0
#define TG_STREAM_5VRAW        1
#define TG_STREAM_FILE_PACKETS 2

/**
 * Data type that can be requested from TG_GetValue().
 */
#define TG_DATA_BATTERY      0
#define TG_DATA_POOR_SIGNAL  1
#define TG_DATA_ATTENTION    2
#define TG_DATA_MEDITATION   3
#define TG_DATA_RAW          4
#define TG_DATA_DELTA        5
#define TG_DATA_THETA        6
#define TG_DATA_ALPHA1       7
#define TG_DATA_ALPHA2       8
#define TG_DATA_BETA1        9
#define TG_DATA_BETA2       10
#define TG_DATA_GAMMA1      11
#define TG_DATA_GAMMA2      12


/**
 * Returns a number indicating the version of the ThinkGear Communications
 * Driver (TGCD) library accessed by this API.  Useful for debugging
 * version-related issues.
 *
 * @return The TGCD library's version number.
 */
THINKGEAR_API int
TG_GetDriverVersion();


/**
 * Returns an ID handle (an int) to a newly-allocated ThinkGear Connection
 * object.  The Connection is used to perform all other operations of this
 * API, so the ID handle is passed as the first argument to all functions
 * of this API.
 *
 * When the ThinkGear Connection is no longer needed, be sure to call
 * TG_FreeConnection() on the ID handle to free its resources.  No more
 * than TG_MAX_CONNECTION_HANDLES Connection handles may exist
 * simultaneously without being freed.
 *
 * @return -1 if too many Connections have been created without being freed
 *         by TG_FreeConnection().
 *
 * @return -2 if there is not enough free memory to allocate to a new
 *         ThinkGear Connection.
 *
 * @return The ID handle of a newly-allocated ThinkGear Connection.
 */
THINKGEAR_API int
TG_GetNewConnectionId();


/**
 * As a ThinkGear Connection reads bytes from its serial stream, it may
 * automatically log those bytes into a log file.  This is useful primarily
 * for debugging purposes.  Calling this function with a valid @c filename
 * will turn this feature on.  Calling this function with an invalid
 * @c filename, or with @c filename set to NULL, will turn this feature
 * off.  This function may be called at any time for either purpose on a
 * ThinkGear Connection.
 *
 * @param connectionId The ID of the ThinkGear Connection to enable stream
 *                     logging for, as obtained from TG_GetNewConnectionId().
 * @param filename     The name of the file to use for stream logging.
 *                     Any existing contents of the file will be erased.
 *                     Set to NULL to disable stream logging by the
 *                     ThinkGear Connection.
 *
 * @return -1 if @c connectionId does not refer to a valid ThinkGear
 *         Connection ID handle.
 *
 * @return -2 if @c filename could not be opened for writing.  You may
 *         check errno for the reason.
 *
 * @return 0 on success.
 */
THINKGEAR_API int
TG_SetStreamLog( int connectionId, const char *filename );


/**
 * As a ThinkGear Connection reads and parses Packets of data from its
 * serial stream, it may log the parsed data into a log file.  This is
 * useful primarily for debugging purposes.  Calling this function with
 * a valid @c filename will turn this feature on.  Calling this function
 * with an invalid @c filename, or with @c filename set to NULL, will turn
 * this feature off.  This function may be called at any time for either
 * purpose on a ThinkGear Connection.
 *
 * @param connectionId The ID of the ThinkGear Connection to enable data
 *                     logging for, as obtained from TG_GetNewConnectionId().
 * @param filename     The name of the file to use for data logging.
 *                     Any existing contents of the file will be erased.
 *                     Set to NULL to disable stream logging by the
 *                     ThinkGear Connection.
 *
 * @return -1 if @c connectionId does not refer to a valid ThinkGear
 *         Connection ID handle.
 *
 * @return -2 if @c filename could not be opened for writing.  You may
 *         check errno for the reason.
 *
 * @return 0 on success.
 */
THINKGEAR_API int
TG_SetDataLog( int connectionId, const char *filename );


/**
 * Connects a ThinkGear Connection, given by @c connectionId, to a serial
 * communication (COM) port in order to communicate with a ThinkGear module.
 * It is important to check the return value of this function before
 * attempting to use the Connection further for other functions in this API.
 *
 * @param connectionId     The ID of the ThinkGear Connection to connect, as
 *                         obtained from TG_GetNewConnectionId().
 * @param serialPortName   The name of the serial communication (COM) stream
 *                         port.  COM ports on PC Windows systems are named
 *                         like '\\.\COM4' (remember that backslashes in
 *                         strings in most programming languages need to be
 *                         escaped), while COM ports on Windows Mobile
 *                         systems are named like 'COM4:' (note the colon at
 *                         the end).  Linux COM ports may be named like
 *                         '/dev/ttys0'.  Refer to the documentation for
 *                         your particular platform to determine the
 *                         available COM port names on your system.
 * @param serialBaudrate   The baudrate to use to attempt to communicate
 *                         on the serial communication port.  Select from
 *                         one of the TG_BAUD_* constants defined above,
 *                         such as TG_BAUD_9600 or TG_BAUD_57600.
 * @param serialDataFormat The type of ThinkGear data stream.  Select from
 *                         one of the TG_STREAM_* constants defined above.
 *                         Most applications should use TG_STREAM_PACKETS
 *                         (the data format for Embedded ThinkGear).
 *                         TG_STREAM_5VRAW is supported only for legacy
 *                         non-embedded purposes.
 *
 * @return -1 if @c connectionId does not refer to a valid ThinkGear
 *         Connection ID handle.
 *
 * @return -2 if @c serialPortName could not be opened as a serial
 *         communication port for any reason.  Check that the name
 *         is a valid COM port on your system.
 *
 * @return -3 if @c serialBaudrate is not a valid TG_BAUD_* value.
 *
 * @return -4 if @c serialDataFormat is not a valid TG_STREAM_* type.
 *
 * @return 0 on success.
 */
THINKGEAR_API int
TG_Connect( int connectionId, const char *serialPortName, int serialBaudrate,
            int serialDataFormat );


/**
 * Attempts to use the ThinkGear Connection, given by @c connectionId,
 * to read @c numPackets of data from the serial stream.  The Connection
 * will (internally) "remember" the most recent value it has seen of
 * each possible ThinkGear data type, so that any subsequent call to
 * @c TG_GetValue() will return the most recently seen values.
 *
 * Set @c numPackets to -1 to attempt to read all Packets of data that
 * may be currently available on the serial stream.
 *
 * @param connectionId The ID of the ThinkGear Connection which should
 *                     read packets from its serial communication stream,
 *                     as obtained from TG_GetNewConnectionId().
 * @param numPackets   The number of data Packets to attempt to read from
 *                     the ThinkGear Connection.  Only the most recently
 *                     read value of each data type will be "remembered"
 *                     by the ThinkGear Connection.  Setting this parameter
 *                     to -1 will attempt to read all currently available
 *                     Packets that are on the data stream.
 *
 * @return -1 if @c connectionId does not refer to a valid ThinkGear
 *         Connection ID handle.
 *
 * @return -2 if there were not even any bytes available to be read from
 *         the Connection's serial communication stream.
 *
 * @return -3 if an I/O error occurs attempting to read from the Connection's
 *         serial communication stream.
 *
 * @return The number of Packets that were successfully read and parsed
 *         from the Connection.
 */
THINKGEAR_API int
TG_ReadPackets( int connectionId, int numPackets );


/**
 * Returns the most recently read value of the given @c dataType, which
 * is one of the TG_DATA_* constants defined above.  Use @c TG_ReadPackets()
 * to read more Packets in order to obtain updated values.  Afterwards, use
 * @c TG_GetValueStatus() to check if a call to @c TG_ReadPackets() actually
 * updated a particular @c dataType.
 *
 * NOTE: This function will terminate the program with a message printed
 * to stderr if @c connectionId is not a valid ThinkGear Connection, or
 * if @c dataType is not a valid TG_DATA_* constant.
 *
 * @param connectionId The ID of the ThinkGear Connection to get a data
 *                     value from, as obtained from TG_GetNewConnectionId().
 * @param dataType     The type of data value desired.  Select from one of
 *                     the TG_DATA_* constants defined above.  Refer to the
 *                     documentation of each constant for details of how
 *                     to interpret its value.
 *
 * @return The most recent value of the requested @c dataType.
 */
THINKGEAR_API float
TG_GetValue( int connectionId, int dataType );


/**
 * Returns Non-zero if the @c dataType was updated by the most recent call
 * to TG_ReadPackets().  Returns 0 otherwise.
 *
 * @param connectionId The ID of the ThinkGear Connection to get a data
 *                     value from, as obtained from TG_GetNewConnectionId().
 * @param dataType     The type of data value desired.  Select from one of
 *                     the TG_DATA_* constants defined above.
 *
 * NOTE: This function will terminate the program with a message printed
 * to stderr if @c connectionId is not a valid ThinkGear Connection, or
 * if @c dataType is not a valid TG_DATA_* constant.
 *
 * @return Non-zero if the @c dataType was updated by the most recent call
 *         to TG_GetValue().  Returns 0 otherwise.
 */
THINKGEAR_API int
TG_GetValueStatus( int connectionId, int dataType );


/**
 * Sends a byte through the ThinkGear Connection (presumably to a ThinkGear
 * module).  This function is intended for advanced ThinkGear Command Byte
 * operations.
 *
 * WARNING: Always make sure at least one valid Packet has been read (i.e.
 *          through the @c TG_ReadPackets() function) at some point BEFORE
 *          calling this function.  This is to ENSURE the Connection is
 *          communicating at the right baud rate.  Sending Command Byte
 *          at the wrong baud rate may put a ThinkGear module into an
 *          indeterminate and inoperable state until it is reset by power
 *          cycling (turning it off and then on again).
 *
 * NOTE: After sending a Command Byte that changes a ThinkGear baud rate,
 *       you will need to call @c TG_SetBaudrate() to change the baud rate
 *       of your @c connectionId as well.  After such a baud rate change,
 *       it is important to check for a valid Packet to be received by
 *       @c TG_ReadPacket() before attempting to send any other Command
 *       Bytes, for the same reasons as describe in the WARNING above.
 *
 * @param connectionId The ID of the ThinkGear Connection to send a byte
 *                     through, as obtained from TG_GetNewConnectionId().
 * @param b            The byte to send through.  Note that only the lowest
 *                     8-bits of the value will actually be sent through.
 *
 * @return -1 if @c connectionId does not refer to a valid ThinkGear
 *         Connection ID handle.
 *
 * @return -2 if @c connectionId is connected to an input file stream instead
 *         of an actual ThinkGear COM stream (i.e. nowhere to send the byte
 *         to).
 *
 * @return -3 if an I/O error occurs attempting to send the byte (i.e. broken
 *         stream connection).
 *
 * @return 0 on success.
 */
THINKGEAR_API int
TG_SendByte( int connectionId, int b );


/**
 * Attempts to change the baud rate of the ThinkGear Connection, given by
 * @c connectionId, to @c serialBaudrate.  This function does not typically
 * need to be called, except after calling @c TG_SendByte() to send a
 * Command Byte that changes the ThinkGear module's baud rate.  See
 * TG_SendByte() for details and NOTE.
 *
 * @param connectionId   The ID of the ThinkGear Connection to send a byte
 *                       through, as obtained from TG_GetNewConnectionId().
 * @param serialBaudrate The baudrate to use to attempt to communicate
 *                       on the serial communication port.  Select from
 *                       one of the TG_BAUD_* constants defined above,
 *                       such as TG_BAUD_9600 or TG_BAUD_57600.
 *                       TG_BAUD_9600 is the typical default baud rate
 *                       for standard ThinkGear modules.
 *
 * @return -1 if @c connectionId does not refer to a valid ThinkGear
 *         Connection ID handle.
 *
 * @return -2 if @c serialBaudrate is not a valid TG_BAUD_* value.
 *
 * @return -3 if an error occurs attempting to set the baud rate.
 *
 * @return -4 if @c connectionId is connected to an input file stream instead
 *         of an actual ThinkGear COM stream.
 *
 * @return 0 on success.
 */
THINKGEAR_API int
TG_SetBaudrate( int connectionId, int serialBaudrate );


/**
 * Attempts to change the data Packet parsing format used by the ThinkGear
 * Connection, given by @c connectionId, to @c serialDataFormat.  This
 * function does not typically need to be called, and is provided only
 * for special testing purposes.
 *
 * @param connectionId     The ID of the ThinkGear Connection to send a byte
 *                         through, as obtained from TG_GetNewConnectionId().
 * @param serialDataFormat The type of ThinkGear data stream.  Select from
 *                         one of the TG_STREAM_* constants defined above.
 *                         Most applications should use TG_STREAM_PACKETS
 *                         (the data format for Embedded ThinkGear modules).
 *                         TG_STREAM_5VRAW is supported only for legacy
 *                         non-embedded purposes.
 *
 * @return -1 if @c connectionId does not refer to a valid ThinkGear
 *         Connection ID handle.
 *
 * @return -2 if @c serialDataFormat is not a valid TG_STREAM_* type.
 *
 * @return 0 on success.
 */
THINKGEAR_API int
TG_SetDataFormat( int connectionId, int serialDataFormat );


/**
 * Disconnects the ThinkGear Connection, given by @c connectionId, from
 * its serial communication (COM) port.  Note that after this call, the
 * Connection will not be valid to use with any of the API functions
 * that require a valid ThinkGear Connection, except TG_SetStreamLog(),
 * TG_SetDataLog(), TG_Connect(), and TG_FreeConnection().
 *
 * Note that TG_FreeConnection() will automatically disconnect a
 * Connection as well, so it is not necessary to call this function
 * unless you wish to reuse the @c connectionId to call TG_Connect()
 * again.
 *
 * @param connectionId The ID of the ThinkGear Connection to disconnect, as
 *                     obtained from TG_GetNewConnectionId().
 */
THINKGEAR_API void
TG_Disconnect( int connectionId );


/**
 * Frees all memory associated with the given ThinkGear Connection.
 *
 * Note that this function will automatically call TG_Disconnect() to
 * disconnect the Connection first, if appropriate, so that it is not
 * necessary to explicitly call TG_Disconnect() at all, unless you wish
 * to reuse the @c connectionId without freeing it first for whatever
 * reason.
 *
 * @param connectionId The ID of the ThinkGear Connection to disconnect, as
 *                     obtained from TG_GetNewConnectionId().
 */
THINKGEAR_API void
TG_FreeConnection( int connectionId );



/* BEGIN FUNCTIONS FOR JNI SUPPORT */
#if defined(COMPILE_FOR_JNI)

#include <jni.h>

/*
 * Class:     ThinkGear
 * Method:    GetDriverVersion
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_ThinkGear_GetDriverVersion( JNIEnv *, jclass );

/*
 * Class:     ThinkGear
 * Method:    GetNewConnectionId
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_ThinkGear_GetNewConnectionId( JNIEnv *, jclass );

/*
 * Class:     ThinkGear
 * Method:    SetStreamLog
 * Signature: (ILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_ThinkGear_SetStreamLog( JNIEnv *, jclass, jint, jstring );

/*
 * Class:     ThinkGear
 * Method:    SetDataLog
 * Signature: (ILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_ThinkGear_SetDataLog( JNIEnv *, jclass, jint, jstring );

/*
 * Class:     ThinkGear
 * Method:    Connect
 * Signature: (ILjava/lang/String;II)I
 */
JNIEXPORT jint JNICALL
Java_ThinkGear_Connect( JNIEnv *, jclass, jint, jstring, jint, jint );

/*
 * Class:     ThinkGear
 * Method:    ReadPackets
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL
Java_ThinkGear_ReadPackets( JNIEnv *, jclass, jint, jint );

/*
 * Class:     ThinkGear
 * Method:    GetValue
 * Signature: (II)D
 */
JNIEXPORT jdouble JNICALL
Java_ThinkGear_GetValue( JNIEnv *, jclass, jint, jint );

/*
 * Class:     ThinkGear
 * Method:    GetValueStatus
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL
Java_ThinkGear_GetValueStatus( JNIEnv *, jclass, jint, jint );

/*
 * Class:     ThinkGear
 * Method:    SendByte
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL
Java_ThinkGear_SendByte( JNIEnv *, jclass, jint, jint );

/*
 * Class:     ThinkGear
 * Method:    SetBaudrate
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL
Java_ThinkGear_SetBaudrate( JNIEnv *, jclass, jint, jint );

/*
 * Class:     ThinkGear
 * Method:    SetDataFormat
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL
Java_ThinkGear_SetDataFormat( JNIEnv *, jclass, jint, jint );

/*
 * Class:     ThinkGear
 * Method:    Disconnect
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_ThinkGear_Disconnect( JNIEnv *, jclass, jint );

/*
 * Class:     ThinkGear
 * Method:    FreeInstance
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_ThinkGear_FreeInstance( JNIEnv *, jclass, jint );

#endif /* COMPILE_FOR_JNI */
/* END FUNCTIONS FOR JNI SUPPORT */


#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* THINKGEAR_H_ */
