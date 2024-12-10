<style>
body {
    text-align:justify;
    max-width: 25cm;
    margin-left: auto;
    margin-right: auto;
    font-family: Georgia;
    counter-reset: h1counter h2counter  h3counter toc1counter toc2counter toc3counter;
 }

table {
   border: 1px solid #e0e0e0;
   border-collapse: collapse;
   margin-bottom: 25px;
}

th, td {
    border: 1px solid #e0e0e0;
    font-family: Courier, monospace;
    font-size: 90%;
    padding: 2px;
}

/*
 * Alternate colors for the table, including the heading row
 */
th {
background-color: #e0e0e0   
}
tr:nth-child(even){background-color: #e0e0e0}

h1, h2, h3, h4, h5 { font-family: Arial; }
h1 { font-size:120%; margin-bottom: 25px; }
h2 { font-size:110%; margin-bottom: 15px; }
h3 { font-size:100%; margin-bottom: 10px;}
h4, li { font-size:100%; }

caption{ font-family:Arial; font-size:85%;}

code, pre, .codelike {
    font-family: Courier, monospace;
    font-size: 100%;
    color: darkblue;
}

/*
 * Counters for the main headings
 */

h1:before {
    counter-increment: h1counter;
    content: counter(h1counter) "\00a0 ";
}
h1 {
    counter-reset: h2counter;
}

h2:before {
    counter-increment: h2counter;
    content: counter(h1counter) "." counter(h2counter) "\00a0 ";
}
h2 {
    counter-reset: h3counter;
}

h3:before {
  counter-increment: h3counter;
  content: counter(h1counter) "." counter(h2counter) "." counter(h3counter) "\00a0 ";
}

/*
 * The document title, centered
 */
doctitle {font-family: Arial; font-size:120%; font-weight: bold; margin-bottom:25px; text-align:center; display:block;}
titlepagetext {text-align:center; display:block;}

/*
 * The table of contents formatting
 */
toctitle {font-family: Arial; font-size:120%; font-weight: bold; margin-bottom:25px; display:block;}
toc1, toc2, toc3 {font-family: Arial; font-size:100%; margin-bottom:2px; display:block;}
toc1 {text-indent: 0px;}
toc2 {text-indent: 15px;}
toc3 {text-indent: 30px;}

toc1:before {
    content: counter(toc1counter) "\00a0 ";
    counter-increment: toc1counter;
}
toc1 {
    counter-reset: toc2counter;
}

toc2:before {
    content: counter(toc1counter) "." counter(toc2counter) "\00a0 ";
    counter-increment: toc2counter;
}
toc2 {
    counter-reset: toc3counter;
}

toc3:before {
  content: counter(toc1counter) "." counter(toc2counter) "." counter(toc3counter) "\00a0 ";
  counter-increment: toc3counter;
}

/* How it looks on a screen, notice the fancy hr blocks and lack of page breaks */
@media screen {
  body {
    background-color: #f0f0f0;
  }
  .page-break { display: none; }
  hr { 
    height: 25px; 
    border-style: solid; 
    border-color: gray; 
    border-width: 1px 0 0 0; 
    border-radius: 10px; 
  } 
  hr:before { 
    display: block; 
    content: ""; 
    height: 25px; 
    margin-top: -26px; 
    border-style: solid; 
    border-color: gray; 
    border-width: 0 0 1px 0; 
    border-radius: 10px; 
  }
}

/* How it looks when printed, hr turned off, in favor of page breaks*/
@media print {
  hr {display: none;}
  body {background-color: white;}
  .page-break{page-break-before: always;}
}
</style>



# CEDAT Protocol

Binary protocol for compressed log data

 Protocol version is 0.1.

 Protocol API is 1.

## Packets enumeration

CEDAT packet definitions

| Name                                                  | Value | Description                                 |
| ----------------------------------------------------- | :---: | ------------------------------------------- |
| [`CEDAT_FILE_INFO`](#CEDAT_FILE_INFO)                 | 1     | Log file version information                |
| [`CEDAT_NEW_VAR`](#CEDAT_NEW_VAR)                     | 128   | New data variable received                  |
| [`CEDAT_DATAPOINT`](#CEDAT_DATAPOINT)                 | 129   | Timestamped datapoint (floating point data) |
| [`CEDAT_DATAPOINT_BOOLEAN`](#CEDAT_DATAPOINT_BOOLEAN) | 130   | Timestamped datapoint (boolean data)        |
[<a name="Packets"></a>Packets enumeration]



## <a name="CEDAT_FILE_INFO"></a>Version packet

Log file version information

- packet identifier: `CEDAT_FILE_INFO` : 1
- minimum data length: 2
- maximum data length: 9


| Bytes | Name         | [Enc](#Enc)                          | Repeat | Description                                                                     |
| ----- | ------------ | :----------------------------------: | :----: | ------------------------------------------------------------------------------- |
| 0     | 1)apiVersion | U8                                   | 1      | Log file API version.<br>Data are given constant value on encode getCEDATApi(). |
| 1...8 | 2)protocol   | Zero-terminated string up to 8 bytes         || Protocol version.<br>Data are given constant value on encode getCEDATVersion(). |
[Version packet bytes]


## <a name="CEDAT_NEW_VAR"></a>NewLogVariable packet

New data variable received

- packet identifier: `CEDAT_NEW_VAR` : 128
- minimum data length: 7
- maximum data length: 196


| Bytes     | Name         | [Enc](#Enc)                           | Repeat | Description  |
| --------- | ------------ | :-----------------------------------: | :----: | ------------ |
| 0...1     | 1)variableId | U16                                   | 1      | Variable ID. |
| 2...3     | 2)ownerId    | I16                                   | 1      |              |
| 4...67    | 3)ownerName  | Zero-terminated string up to 64 bytes         ||              |
| 68...131  | 4)title      | Zero-terminated string up to 64 bytes         ||              |
| 132...195 | 5)units      | Zero-terminated string up to 64 bytes         ||              |
[NewLogVariable packet bytes]


## <a name="CEDAT_DATAPOINT"></a>TimestampedData packet

Timestamped datapoint (floating point data)

- packet identifier: `CEDAT_DATAPOINT` : 129
- data length: 7


| Bytes | Name         | [Enc](#Enc) | Repeat | Description                           |
| ----- | ------------ | :---------: | :----: | ------------------------------------- |
| 0:7   | 1)reserved   | B1          | 1      | Reserved bit (ignored).               |
| 0:6   | 2)delta      | B1          | 1      | 1 if the data value is delta-encoded. |
| 1...2 | 3)variableId | U16         | 1      | Variable ID.                          |
| 3...6 | 4)value      | F32         | 1      | Value.                                |
[TimestampedData packet bytes]


## <a name="CEDAT_DATAPOINT_BOOLEAN"></a>TimestampedBooleanData packet

Timestamped datapoint (boolean data)

- packet identifier: `CEDAT_DATAPOINT_BOOLEAN` : 130
- data length: 3


| Bytes     | Name         | [Enc](#Enc) | Repeat | Description                                                                                                  |
| --------- | ------------ | :---------: | :----: | ------------------------------------------------------------------------------------------------------------ |
| 0:7       | 1)reserved   | B1          | 1      | Reserved bit (ignored).                                                                                      |
| 0:6       | 2)delta      | B1          | 1      | 1 if the data value is delta-encoded (false for boolean data).<br>Data are given constant value on encode 0. |
| 0:5...0:1 | 3)reserved2  | B5          | 1      | Reserved for future use.<br>Data are given constant value on encode 0.                                       |
| 0:0       | 4)value      | B1          | 1      | Value.                                                                                                       |
| 1...2     | 5)variableId | U16         | 1      | Variable ID.                                                                                                 |
[TimestampedBooleanData packet bytes]

<div class="page-break"></div>


----------------------------

# About this ICD

This is the interface control document for data *on the wire*, not data in memory. This document was automatically generated by the [ProtoGen software](https://github.com/billvaglienti/ProtoGen), version 3.2.a. ProtoGen also generates C source code for doing most of the work of encoding data from memory to the wire, and vice versa.

## Encodings

Data for this protocol are sent in BIG endian format. Any field larger than one byte is sent with the most signficant byte first, and the least significant byte last.

Data can be encoded as unsigned integers, signed integers (two's complement), bitfields, and floating point.

| <a name="Enc"></a>Encoding | Interpretation                        | Notes                                                                       |
| :--------------------------: | ------------------------------------- | --------------------------------------------------------------------------- |
| UX                           | Unsigned integer X bits long          | X must be: 8, 16, 24, 32, 40, 48, 56, or 64                                 |
| IX                           | Signed integer X bits long            | X must be: 8, 16, 24, 32, 40, 48, 56, or 64                                 |
| BX                           | Unsigned integer bitfield X bits long | X must be greater than 0 and less than 32                                   |
| F16:X                        | 16 bit float with X significand bits  | 1 sign bit : 15-X exponent bits : X significant bits with implied leading 1 |
| F24:X                        | 24 bit float with X significand bits  | 1 sign bit : 23-X exponent bits : X significant bits with implied leading 1 |
| F32                          | 32 bit float (IEEE-754)               | 1 sign bit : 8 exponent bits : 23 significant bits with implied leading 1   |
| F64                          | 64 bit float (IEEE-754)               | 1 sign bit : 11 exponent bits : 52 significant bits with implied leading 1  |

## Size of fields
The encoding tables give the bytes for each field as X...Y; where X is the first byte (counting from 0) and Y is the last byte. For example a 4 byte field at the beginning of a packet will give 0...3. If the field is 1 byte long then only the starting byte is given. Bitfields are more complex, they are displayed as Byte:Bit. A 3-bit bitfield at the beginning of a packet will give 0:7...0:5, indicating that the bitfield uses bits 7, 6, and 5 of byte 0. Note that the most signficant bit of a byte is 7, and the least signficant bit is 0. If the bitfield is 1 bit long then only the starting bit is given.

The byte count in the encoding tables are based on the maximum length of the field(s). If a field is variable length then the actual byte location of the data may be different depending on the value of the variable field. If the field is a variable length character string this will be indicated in the encoding column of the table. If the field is a variable length array this will be indicated in the repeat column of the encoding table. If the field depends on the non-zero value of another field this will be indicated in the description column of the table.

