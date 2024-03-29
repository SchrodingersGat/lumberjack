// CEDATPackets.cpp was generated by ProtoGen version 3.2.a

#include "CEDATPackets.hpp"
#include "fielddecode.hpp"
#include "fieldencode.hpp"
#include "scaleddecode.hpp"
#include "scaledencode.hpp"

/*!
 * Construct a CEDAT_Version_t
 */
CEDAT_Version_t::CEDAT_Version_t(void) :
    apiVersion((uint8_t)getCEDATApi()),
    protocol(getCEDATVersion())
{
}// CEDAT_Version_t::CEDAT_Version_t

/*!
 * \brief Decode the CEDAT_Version packet
 *
 * Log file version information
 * \param _pg_pkt points to the packet being decoded by this function
 * \return false is returned if the packet ID or size is wrong, else true
 */
bool CEDAT_Version_t::decode(const void* _pg_pkt)
{
    int _pg_numbytes;
    int _pg_byteindex = 0;
    const uint8_t* _pg_data;

    // Verify the packet identifier
    if(getCEDATPacketID(_pg_pkt) != id())
        return false;

    // Verify the packet size
    _pg_numbytes = getCEDATPacketSize(_pg_pkt);
    if(_pg_numbytes < minLength())
        return false;

    // The raw data from the packet
    _pg_data = getCEDATPacketDataConst(_pg_pkt);

    // Log file API version
    // Range of apiVersion is 0 to 255.
    apiVersion = uint8FromBytes(_pg_data, &_pg_byteindex);

    // Protocol version
    stringFromBytes(protocol, _pg_data, &_pg_byteindex, 8, 0);

    return true;

}// CEDAT_Version_t::decode

/*!
 * \brief Create the CEDAT_Version packet
 *
 * Log file version information
 * \param _pg_pkt points to the packet which will be created by this function
 */
void CEDAT_Version_t::encode(void* _pg_pkt)
{
    uint8_t* _pg_data = getCEDATPacketData(_pg_pkt);
    int _pg_byteindex = 0;

    // Log file API version
    uint8ToBytes((uint8_t)(getCEDATApi()), _pg_data, &_pg_byteindex);

    // Protocol version
    stringToBytes(getCEDATVersion(), _pg_data, &_pg_byteindex, 8, 0);

    // complete the process of creating the packet
    finishCEDATPacket(_pg_pkt, _pg_byteindex, id());

}// CEDAT_Version_t::encode

/*!
 * \brief Decode the CEDAT_Version packet
 *
 * Log file version information
 * \param _pg_pkt points to the packet being decoded by this function
 * \param apiVersion receives Log file API version
 * \param protocol receives Protocol version
 * \return false is returned if the packet ID or size is wrong, else true
 */
bool CEDAT_Version_t::decode(const void* _pg_pkt, uint8_t* apiVersion, char protocol[8])
{
    int _pg_byteindex = 0;
    const uint8_t* _pg_data = getCEDATPacketDataConst(_pg_pkt);
    int _pg_numbytes = getCEDATPacketSize(_pg_pkt);

    // Verify the packet identifier
    if(getCEDATPacketID(_pg_pkt) != id())
        return 0;

    if(_pg_numbytes < minLength())
        return 0;

    // Log file API version
    // Range of apiVersion is 0 to 255.
    (*apiVersion) = uint8FromBytes(_pg_data, &_pg_byteindex);

    // Protocol version
    stringFromBytes(protocol, _pg_data, &_pg_byteindex, 8, 0);

    return 1;

}// CEDAT_Version_t::decode

/*!
 * Construct a CEDAT_NewLogVariable_t
 */
CEDAT_NewLogVariable_t::CEDAT_NewLogVariable_t(void) :
    variableId(0),
    ownerId(0),
    ownerName(""),
    title(""),
    units("")
{
}// CEDAT_NewLogVariable_t::CEDAT_NewLogVariable_t

/*!
 * \brief Create the CEDAT_NewLogVariable packet
 *
 * New data variable received
 * \param _pg_pkt points to the packet which will be created by this function
 */
void CEDAT_NewLogVariable_t::encode(void* _pg_pkt) const
{
    uint8_t* _pg_data = getCEDATPacketData(_pg_pkt);
    int _pg_byteindex = 0;

    // Variable ID
    // Range of variableId is 0 to 65535.
    uint16ToBeBytes(variableId, _pg_data, &_pg_byteindex);

    // Range of ownerId is -32768 to 32767.
    int16ToBeBytes(ownerId, _pg_data, &_pg_byteindex);

    stringToBytes(ownerName, _pg_data, &_pg_byteindex, 64, 0);

    stringToBytes(title, _pg_data, &_pg_byteindex, 64, 0);

    stringToBytes(units, _pg_data, &_pg_byteindex, 64, 0);

    // complete the process of creating the packet
    finishCEDATPacket(_pg_pkt, _pg_byteindex, id());

}// CEDAT_NewLogVariable_t::encode

/*!
 * \brief Decode the CEDAT_NewLogVariable packet
 *
 * New data variable received
 * \param _pg_pkt points to the packet being decoded by this function
 * \return false is returned if the packet ID or size is wrong, else true
 */
bool CEDAT_NewLogVariable_t::decode(const void* _pg_pkt)
{
    int _pg_numbytes;
    int _pg_byteindex = 0;
    const uint8_t* _pg_data;

    // Verify the packet identifier
    if(getCEDATPacketID(_pg_pkt) != id())
        return false;

    // Verify the packet size
    _pg_numbytes = getCEDATPacketSize(_pg_pkt);
    if(_pg_numbytes < minLength())
        return false;

    // The raw data from the packet
    _pg_data = getCEDATPacketDataConst(_pg_pkt);

    // Variable ID
    // Range of variableId is 0 to 65535.
    variableId = uint16FromBeBytes(_pg_data, &_pg_byteindex);

    // Range of ownerId is -32768 to 32767.
    ownerId = int16FromBeBytes(_pg_data, &_pg_byteindex);

    stringFromBytes(ownerName, _pg_data, &_pg_byteindex, 64, 0);

    stringFromBytes(title, _pg_data, &_pg_byteindex, 64, 0);

    stringFromBytes(units, _pg_data, &_pg_byteindex, 64, 0);

    return true;

}// CEDAT_NewLogVariable_t::decode

/*!
 * \brief Create the CEDAT_NewLogVariable packet
 *
 * New data variable received
 * \param _pg_pkt points to the packet which will be created by this function
 * \param variableId is Variable ID
 * \param ownerId is 
 * \param ownerName is 
 * \param title is 
 * \param units is 
 */
void CEDAT_NewLogVariable_t::encode(void* _pg_pkt, uint16_t variableId, int16_t ownerId, const char ownerName[64], const char title[64], const char units[64])
{
    uint8_t* _pg_data = getCEDATPacketData(_pg_pkt);
    int _pg_byteindex = 0;

    // Variable ID
    // Range of variableId is 0 to 65535.
    uint16ToBeBytes(variableId, _pg_data, &_pg_byteindex);

    // Range of ownerId is -32768 to 32767.
    int16ToBeBytes(ownerId, _pg_data, &_pg_byteindex);

    stringToBytes(ownerName, _pg_data, &_pg_byteindex, 64, 0);

    stringToBytes(title, _pg_data, &_pg_byteindex, 64, 0);

    stringToBytes(units, _pg_data, &_pg_byteindex, 64, 0);

    // complete the process of creating the packet
    finishCEDATPacket(_pg_pkt, _pg_byteindex, id());

}// CEDAT_NewLogVariable_t::encode

/*!
 * \brief Decode the CEDAT_NewLogVariable packet
 *
 * New data variable received
 * \param _pg_pkt points to the packet being decoded by this function
 * \param variableId receives Variable ID
 * \param ownerId receives 
 * \param ownerName receives 
 * \param title receives 
 * \param units receives 
 * \return false is returned if the packet ID or size is wrong, else true
 */
bool CEDAT_NewLogVariable_t::decode(const void* _pg_pkt, uint16_t* variableId, int16_t* ownerId, char ownerName[64], char title[64], char units[64])
{
    int _pg_byteindex = 0;
    const uint8_t* _pg_data = getCEDATPacketDataConst(_pg_pkt);
    int _pg_numbytes = getCEDATPacketSize(_pg_pkt);

    // Verify the packet identifier
    if(getCEDATPacketID(_pg_pkt) != id())
        return 0;

    if(_pg_numbytes < minLength())
        return 0;

    // Variable ID
    // Range of variableId is 0 to 65535.
    (*variableId) = uint16FromBeBytes(_pg_data, &_pg_byteindex);

    // Range of ownerId is -32768 to 32767.
    (*ownerId) = int16FromBeBytes(_pg_data, &_pg_byteindex);

    stringFromBytes(ownerName, _pg_data, &_pg_byteindex, 64, 0);

    stringFromBytes(title, _pg_data, &_pg_byteindex, 64, 0);

    stringFromBytes(units, _pg_data, &_pg_byteindex, 64, 0);

    return 1;

}// CEDAT_NewLogVariable_t::decode

/*!
 * Construct a CEDAT_TimestampedData_t
 */
CEDAT_TimestampedData_t::CEDAT_TimestampedData_t(void) :
    reserved(0),
    delta(0),
    variableId(0),
    value(0.0f)
{
}// CEDAT_TimestampedData_t::CEDAT_TimestampedData_t

/*!
 * \brief Create the CEDAT_TimestampedData packet
 *
 * Timestamped datapoint (floating point data)
 * \param _pg_pkt points to the packet which will be created by this function
 */
void CEDAT_TimestampedData_t::encode(void* _pg_pkt) const
{
    uint8_t* _pg_data = getCEDATPacketData(_pg_pkt);
    int _pg_byteindex = 0;

    // Reserved bit (ignored)
    _pg_data[_pg_byteindex] = (uint8_t)reserved << 7;

    // 1 if the data value is delta-encoded
    _pg_data[_pg_byteindex] |= (uint8_t)delta << 6;
    _pg_byteindex += 1; // close bit field


    // Variable ID
    // Range of variableId is 0 to 65535.
    uint16ToBeBytes(variableId, _pg_data, &_pg_byteindex);

    // Value
    // Range of value is -3.402823466e+38f to 3.402823466e+38f.
    float32ToBeBytes((float)value, _pg_data, &_pg_byteindex);

    // complete the process of creating the packet
    finishCEDATPacket(_pg_pkt, _pg_byteindex, id());

}// CEDAT_TimestampedData_t::encode

/*!
 * \brief Decode the CEDAT_TimestampedData packet
 *
 * Timestamped datapoint (floating point data)
 * \param _pg_pkt points to the packet being decoded by this function
 * \return false is returned if the packet ID or size is wrong, else true
 */
bool CEDAT_TimestampedData_t::decode(const void* _pg_pkt)
{
    int _pg_numbytes;
    int _pg_byteindex = 0;
    const uint8_t* _pg_data;

    // Verify the packet identifier
    if(getCEDATPacketID(_pg_pkt) != id())
        return false;

    // Verify the packet size
    _pg_numbytes = getCEDATPacketSize(_pg_pkt);
    if(_pg_numbytes < minLength())
        return false;

    // The raw data from the packet
    _pg_data = getCEDATPacketDataConst(_pg_pkt);

    // Reserved bit (ignored)
    reserved = (_pg_data[_pg_byteindex] >> 7);

    // 1 if the data value is delta-encoded
    delta = ((_pg_data[_pg_byteindex] >> 6) & 0x1);
    _pg_byteindex += 1; // close bit field

    // Variable ID
    // Range of variableId is 0 to 65535.
    variableId = uint16FromBeBytes(_pg_data, &_pg_byteindex);

    // Value
    // Range of value is -3.402823466e+38f to 3.402823466e+38f.
    value = float32FromBeBytes(_pg_data, &_pg_byteindex);

    return true;

}// CEDAT_TimestampedData_t::decode

/*!
 * \brief Create the CEDAT_TimestampedData packet
 *
 * Timestamped datapoint (floating point data)
 * \param _pg_pkt points to the packet which will be created by this function
 * \param reserved is Reserved bit (ignored)
 * \param delta is 1 if the data value is delta-encoded
 * \param variableId is Variable ID
 * \param value is Value
 */
void CEDAT_TimestampedData_t::encode(void* _pg_pkt, unsigned reserved, unsigned delta, uint16_t variableId, float value)
{
    uint8_t* _pg_data = getCEDATPacketData(_pg_pkt);
    int _pg_byteindex = 0;

    // Reserved bit (ignored)
    _pg_data[_pg_byteindex] = (uint8_t)reserved << 7;

    // 1 if the data value is delta-encoded
    _pg_data[_pg_byteindex] |= (uint8_t)delta << 6;
    _pg_byteindex += 1; // close bit field

    // Variable ID
    // Range of variableId is 0 to 65535.
    uint16ToBeBytes(variableId, _pg_data, &_pg_byteindex);

    // Value
    // Range of value is -3.402823466e+38f to 3.402823466e+38f.
    float32ToBeBytes((float)value, _pg_data, &_pg_byteindex);

    // complete the process of creating the packet
    finishCEDATPacket(_pg_pkt, _pg_byteindex, id());

}// CEDAT_TimestampedData_t::encode

/*!
 * \brief Decode the CEDAT_TimestampedData packet
 *
 * Timestamped datapoint (floating point data)
 * \param _pg_pkt points to the packet being decoded by this function
 * \param reserved receives Reserved bit (ignored)
 * \param delta receives 1 if the data value is delta-encoded
 * \param variableId receives Variable ID
 * \param value receives Value
 * \return false is returned if the packet ID or size is wrong, else true
 */
bool CEDAT_TimestampedData_t::decode(const void* _pg_pkt, unsigned* reserved, unsigned* delta, uint16_t* variableId, float* value)
{
    int _pg_byteindex = 0;
    const uint8_t* _pg_data = getCEDATPacketDataConst(_pg_pkt);
    int _pg_numbytes = getCEDATPacketSize(_pg_pkt);

    // Verify the packet identifier
    if(getCEDATPacketID(_pg_pkt) != id())
        return 0;

    if(_pg_numbytes < minLength())
        return 0;

    // Reserved bit (ignored)
    (*reserved) = (_pg_data[_pg_byteindex] >> 7);

    // 1 if the data value is delta-encoded
    (*delta) = ((_pg_data[_pg_byteindex] >> 6) & 0x1);
    _pg_byteindex += 1; // close bit field

    // Variable ID
    // Range of variableId is 0 to 65535.
    (*variableId) = uint16FromBeBytes(_pg_data, &_pg_byteindex);

    // Value
    // Range of value is -3.402823466e+38f to 3.402823466e+38f.
    (*value) = float32FromBeBytes(_pg_data, &_pg_byteindex);

    return 1;

}// CEDAT_TimestampedData_t::decode

/*!
 * Construct a CEDAT_TimestampedBooleanData_t
 */
CEDAT_TimestampedBooleanData_t::CEDAT_TimestampedBooleanData_t(void) :
    reserved(0),
    delta(0),
    reserved2(0),
    value(0),
    variableId(0)
{
}// CEDAT_TimestampedBooleanData_t::CEDAT_TimestampedBooleanData_t

/*!
 * \brief Create the CEDAT_TimestampedBooleanData packet
 *
 * Timestamped datapoint (boolean data)
 * \param _pg_pkt points to the packet which will be created by this function
 */
void CEDAT_TimestampedBooleanData_t::encode(void* _pg_pkt) const
{
    uint8_t* _pg_data = getCEDATPacketData(_pg_pkt);
    int _pg_byteindex = 0;

    // Reserved bit (ignored)
    _pg_data[_pg_byteindex] = (uint8_t)reserved << 7;

    // 1 if the data value is delta-encoded (false for boolean data)

    // Reserved for future use

    // Value
    _pg_data[_pg_byteindex] |= (uint8_t)((value == true) ? 1 : 0);
    _pg_byteindex += 1; // close bit field


    // Variable ID
    // Range of variableId is 0 to 65535.
    uint16ToBeBytes(variableId, _pg_data, &_pg_byteindex);

    // complete the process of creating the packet
    finishCEDATPacket(_pg_pkt, _pg_byteindex, id());

}// CEDAT_TimestampedBooleanData_t::encode

/*!
 * \brief Decode the CEDAT_TimestampedBooleanData packet
 *
 * Timestamped datapoint (boolean data)
 * \param _pg_pkt points to the packet being decoded by this function
 * \return false is returned if the packet ID or size is wrong, else true
 */
bool CEDAT_TimestampedBooleanData_t::decode(const void* _pg_pkt)
{
    int _pg_numbytes;
    int _pg_byteindex = 0;
    const uint8_t* _pg_data;

    // Verify the packet identifier
    if(getCEDATPacketID(_pg_pkt) != id())
        return false;

    // Verify the packet size
    _pg_numbytes = getCEDATPacketSize(_pg_pkt);
    if(_pg_numbytes < minLength())
        return false;

    // The raw data from the packet
    _pg_data = getCEDATPacketDataConst(_pg_pkt);

    // Reserved bit (ignored)
    reserved = (_pg_data[_pg_byteindex] >> 7);

    // 1 if the data value is delta-encoded (false for boolean data)
    delta = ((_pg_data[_pg_byteindex] >> 6) & 0x1);

    // Reserved for future use
    // Range of reserved2 is 0 to 31.
    reserved2 = ((_pg_data[_pg_byteindex] >> 1) & 0x1F);

    // Value
    value = (((_pg_data[_pg_byteindex]) & 0x1)) ? true : false;
    _pg_byteindex += 1; // close bit field

    // Variable ID
    // Range of variableId is 0 to 65535.
    variableId = uint16FromBeBytes(_pg_data, &_pg_byteindex);

    return true;

}// CEDAT_TimestampedBooleanData_t::decode

/*!
 * \brief Create the CEDAT_TimestampedBooleanData packet
 *
 * Timestamped datapoint (boolean data)
 * \param _pg_pkt points to the packet which will be created by this function
 * \param reserved is Reserved bit (ignored)
 * \param value is Value
 * \param variableId is Variable ID
 */
void CEDAT_TimestampedBooleanData_t::encode(void* _pg_pkt, unsigned reserved, bool value, uint16_t variableId)
{
    uint8_t* _pg_data = getCEDATPacketData(_pg_pkt);
    int _pg_byteindex = 0;

    // Reserved bit (ignored)
    _pg_data[_pg_byteindex] = (uint8_t)reserved << 7;

    // 1 if the data value is delta-encoded (false for boolean data)

    // Reserved for future use

    // Value
    _pg_data[_pg_byteindex] |= (uint8_t)((value == true) ? 1 : 0);
    _pg_byteindex += 1; // close bit field

    // Variable ID
    // Range of variableId is 0 to 65535.
    uint16ToBeBytes(variableId, _pg_data, &_pg_byteindex);

    // complete the process of creating the packet
    finishCEDATPacket(_pg_pkt, _pg_byteindex, id());

}// CEDAT_TimestampedBooleanData_t::encode

/*!
 * \brief Decode the CEDAT_TimestampedBooleanData packet
 *
 * Timestamped datapoint (boolean data)
 * \param _pg_pkt points to the packet being decoded by this function
 * \param reserved receives Reserved bit (ignored)
 * \param delta receives 1 if the data value is delta-encoded (false for boolean data)
 * \param reserved2 receives Reserved for future use
 * \param value receives Value
 * \param variableId receives Variable ID
 * \return false is returned if the packet ID or size is wrong, else true
 */
bool CEDAT_TimestampedBooleanData_t::decode(const void* _pg_pkt, unsigned* reserved, unsigned* delta, unsigned* reserved2, bool* value, uint16_t* variableId)
{
    int _pg_byteindex = 0;
    const uint8_t* _pg_data = getCEDATPacketDataConst(_pg_pkt);
    int _pg_numbytes = getCEDATPacketSize(_pg_pkt);

    // Verify the packet identifier
    if(getCEDATPacketID(_pg_pkt) != id())
        return 0;

    if(_pg_numbytes < minLength())
        return 0;

    // Reserved bit (ignored)
    (*reserved) = (_pg_data[_pg_byteindex] >> 7);

    // 1 if the data value is delta-encoded (false for boolean data)
    (*delta) = ((_pg_data[_pg_byteindex] >> 6) & 0x1);

    // Reserved for future use
    // Range of reserved2 is 0 to 31.
    (*reserved2) = ((_pg_data[_pg_byteindex] >> 1) & 0x1F);

    // Value
    (*value) = (((_pg_data[_pg_byteindex]) & 0x1)) ? true : false;
    _pg_byteindex += 1; // close bit field

    // Variable ID
    // Range of variableId is 0 to 65535.
    (*variableId) = uint16FromBeBytes(_pg_data, &_pg_byteindex);

    return 1;

}// CEDAT_TimestampedBooleanData_t::decode
// end of CEDATPackets.cpp
