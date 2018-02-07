[![Build Status](https://travis-ci.org/AdrianPeniak/bitstream.svg?branch=master)](http://travis-ci.org/AdrianPeniak/bitstream)

# Bitstream

small utility functions for serialization and de-serialization of data forked and edited from https://github.com/marcmo/bitstream 

## Example Usecase

write or read incoming/outgoing datastreams for protocols

## Usage

### Writing bits to a stream

    std::vector<uint8_t> vec{};
    BiteStream bs(std::move(vec));
    bs.put<uint8_t>(13,4);
    bs.put<uint8_t>(45,7);
    bs.put<uint8_t>(6,3);
    bs.put<uint16_t>(16385,16);

### Reading bits from a stream

    std::vector<uint8_t> vec{213,185,0,5,8,3,1,2,3,4,5};
    BiteStream bs(std::move(vec));
    std::cout << (int)bs.get<uint8_t>(4) << std::endl;
    std::cout << (int)bs.get<uint8_t>(7) << std::endl;
    std::cout << (int)bs.get<uint8_t>(3) << std::endl;
    std::cout << (int)bs.get<uint16_t>(16) << std::endl;


### Example: Parsing Custom Package

             0                   1                   2                   3
             0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
           +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     header|    msgType    |     msgID     |            msgBSD             |
           +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     header|                            msgSize                            |
           +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     header|                           msgStamp                            |
           +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
    payload|                          msgPayload                           |
           +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    payload|                          msgPayload                           |
           +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    payload|                          msgPayload                           |
           +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+   

Writing the code for the parser is now straight forward:

    std::vector<uint8_t> msg{2,1,0,0,0,0,0,192,90,123,112,175,72,101,108,108,111,32,119,111,114,108,100,0};
    BiteStream bs(std::move(msg));
    std::cout << "msgType\t" << (int)bs.get<uint8_t>() << std::endl;
    std::cout << "msgID\t" << (int)bs.get<uint8_t>(8) << std::endl;
    std::cout <<"msgBSD\t" <<  bs.get<uint16_t>() << std::endl;
    std::cout <<"msgSize\t" <<  bs.get<uint32_t>(32) << std::endl;
    std::cout <<"msgStamp\t" <<  bs.get<uint32_t>() << std::endl;
    auto v = bs.getRest();
    std::cout <<"msgPayload\t" <<  std::string((char*)v.data(), v.size()) << std::endl;

