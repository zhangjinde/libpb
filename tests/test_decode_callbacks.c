/* Decoding testcase for callback fields.
 * Run e.g. ./test_encode_callbacks | ./test_decode_callbacks
 */

#include <stdio.h>
#include <pb/decode.h>
#include "callbacks.pb.h"

bool print_string(pb_istream_t *stream, const pb_field_t *field, void *arg)
{
    uint8_t buffer[1024] = {0};
    
    /* We could read block-by-block to avoid the large buffer... */
    if (stream->bytes_left > sizeof(buffer) - 1)
        return false;
    
    if (!pb_read(stream, buffer, stream->bytes_left))
        return false;
    
    /* Print the string, in format comparable with protoc --decode.
     * Format comes from the arg defined in main().
     */
    printf((char*)arg, buffer);
    return true;
}

bool print_int32(pb_istream_t *stream, const pb_field_t *field, void *arg)
{
    uint64_t value;
    if (!pb_decode_varint(stream, &value))
        return false;
    
    printf((char*)arg, (int32_t)value);
    return true;
}

bool print_fixed32(pb_istream_t *stream, const pb_field_t *field, void *arg)
{
    uint32_t value;
    if (!pb_dec_fixed32(stream, NULL, &value))
        return false;
    
    printf((char*)arg, value);
    return true;
}

bool print_fixed64(pb_istream_t *stream, const pb_field_t *field, void *arg)
{
    uint64_t value;
    if (!pb_dec_fixed64(stream, NULL, &value))
        return false;
    
    printf((char*)arg, value);
    return true;
}

int main()
{
    uint8_t buffer[1024];
    size_t length = fread(buffer, 1, 1024, stdin);
    pb_istream_t stream = pb_istream_from_buffer(buffer, length);
    
    /* Note: empty initializer list initializes the struct with all-0.
     * This is recommended so that unused callbacks are set to NULL instead
     * of crashing at runtime.
     */
    TestMessage testmessage = {};
    
    testmessage.submsg.stringvalue.funcs.decode = &print_string;
    testmessage.submsg.stringvalue.arg = "submsg {\n  stringvalue: \"%s\"\n";
    testmessage.submsg.int32value.funcs.decode = &print_int32;
    testmessage.submsg.int32value.arg = "  int32value: %d\n";
    testmessage.submsg.fixed32value.funcs.decode = &print_fixed32;
    testmessage.submsg.fixed32value.arg = "  fixed32value: %d\n";
    testmessage.submsg.fixed64value.funcs.decode = &print_fixed64;
    testmessage.submsg.fixed64value.arg = "  fixed64value: %lld\n}\n";
    
    testmessage.stringvalue.funcs.decode = &print_string;
    testmessage.stringvalue.arg = "stringvalue: \"%s\"\n";
    testmessage.int32value.funcs.decode = &print_int32;
    testmessage.int32value.arg = "int32value: %d\n";
    testmessage.fixed32value.funcs.decode = &print_fixed32;
    testmessage.fixed32value.arg = "fixed32value: %d\n";
    testmessage.fixed64value.funcs.decode = &print_fixed64;
    testmessage.fixed64value.arg = "fixed64value: %lld\n";
    
    if (!pb_decode(&stream, TestMessage_fields, &testmessage))
        return 1;
    
    return 0;
}
