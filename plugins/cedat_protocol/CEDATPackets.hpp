// CEDATPackets.hpp was generated by ProtoGen version 3.2.a

#ifndef _CEDATPACKETS_HPP
#define _CEDATPACKETS_HPP

// Language target is C++

/*!
 * \file
 */

#include "CEDATProtocol.hpp"

/*!
 * Log file version information
 */
class CEDAT_Version_t
{
public:

    //! Construct a CEDAT_Version_t
    CEDAT_Version_t(void);

    //! \return the identifier for the packet
    static uint32_t id(void) { return CEDAT_FILE_INFO;}

    //! \return the minimum encoded length for the packet
    static int minLength(void) { return (2);}

    //! \return the maximum encoded length for the packet
    static int maxLength(void) { return (9);}

    //! Create the CEDAT_Version packet from parameters
    static void encode(void* pkt);

    //! Decode the CEDAT_Version packet to parameters
    static bool decode(const void* pkt, uint8_t* apiVersion, char protocol[8]);

    //! Decode the CEDAT_Version packet
    bool decode(const void* pkt);

    uint8_t apiVersion;  //!< Log file API version. Field is encoded constant.
    char    protocol[8]; //!< Protocol version. Field is encoded constant.

}; // CEDAT_Version_t

/*!
 * New data variable received
 */
class CEDAT_NewLogVariable_t
{
public:

    //! Construct a CEDAT_NewLogVariable_t
    CEDAT_NewLogVariable_t(void);

    //! \return the identifier for the packet
    static uint32_t id(void) { return CEDAT_NEW_VAR;}

    //! \return the minimum encoded length for the packet
    static int minLength(void) { return (7);}

    //! \return the maximum encoded length for the packet
    static int maxLength(void) { return (196);}

    //! Create the CEDAT_NewLogVariable packet from parameters
    static void encode(void* pkt, uint16_t variableId, int16_t ownerId, const char ownerName[64], const char title[64], const char units[64]);

    //! Decode the CEDAT_NewLogVariable packet to parameters
    static bool decode(const void* pkt, uint16_t* variableId, int16_t* ownerId, char ownerName[64], char title[64], char units[64]);

    //! Create the CEDAT_NewLogVariable packet
    void encode(void* pkt) const;

    //! Decode the CEDAT_NewLogVariable packet
    bool decode(const void* pkt);

    uint16_t variableId;    //!< Variable ID
    int16_t  ownerId;      
    char     ownerName[64];
    char     title[64];    
    char     units[64];    

}; // CEDAT_NewLogVariable_t

/*!
 * Timestamped datapoint (floating point data)
 */
class CEDAT_TimestampedData_t
{
public:

    //! Construct a CEDAT_TimestampedData_t
    CEDAT_TimestampedData_t(void);

    //! \return the identifier for the packet
    static uint32_t id(void) { return CEDAT_DATAPOINT;}

    //! \return the minimum encoded length for the packet
    static int minLength(void) { return (7);}

    //! \return the maximum encoded length for the packet
    static int maxLength(void) { return (7);}

    //! Create the CEDAT_TimestampedData packet from parameters
    static void encode(void* pkt, unsigned reserved, unsigned delta, uint16_t variableId, float value);

    //! Decode the CEDAT_TimestampedData packet to parameters
    static bool decode(const void* pkt, unsigned* reserved, unsigned* delta, uint16_t* variableId, float* value);

    //! Create the CEDAT_TimestampedData packet
    void encode(void* pkt) const;

    //! Decode the CEDAT_TimestampedData packet
    bool decode(const void* pkt);

    unsigned reserved : 1; //!< Reserved bit (ignored)
    unsigned delta : 1;    //!< 1 if the data value is delta-encoded
    uint16_t variableId;   //!< Variable ID
    float    value;        //!< Value

}; // CEDAT_TimestampedData_t

/*!
 * Timestamped datapoint (boolean data)
 */
class CEDAT_TimestampedBooleanData_t
{
public:

    //! Construct a CEDAT_TimestampedBooleanData_t
    CEDAT_TimestampedBooleanData_t(void);

    //! \return the identifier for the packet
    static uint32_t id(void) { return CEDAT_DATAPOINT_BOOLEAN;}

    //! \return the minimum encoded length for the packet
    static int minLength(void) { return (3);}

    //! \return the maximum encoded length for the packet
    static int maxLength(void) { return (3);}

    //! Create the CEDAT_TimestampedBooleanData packet from parameters
    static void encode(void* pkt, unsigned reserved, bool value, uint16_t variableId);

    //! Decode the CEDAT_TimestampedBooleanData packet to parameters
    static bool decode(const void* pkt, unsigned* reserved, unsigned* delta, unsigned* reserved2, bool* value, uint16_t* variableId);

    //! Create the CEDAT_TimestampedBooleanData packet
    void encode(void* pkt) const;

    //! Decode the CEDAT_TimestampedBooleanData packet
    bool decode(const void* pkt);

    unsigned reserved : 1;  //!< Reserved bit (ignored)
    unsigned delta : 1;     //!< 1 if the data value is delta-encoded (false for boolean data). Field is encoded constant.
    unsigned reserved2 : 5; //!< Reserved for future use. Field is encoded constant.
    bool     value;         //!< Value
    uint16_t variableId;    //!< Variable ID

}; // CEDAT_TimestampedBooleanData_t

#endif // _CEDATPACKETS_HPP