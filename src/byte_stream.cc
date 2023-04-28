#include <stdexcept>

#include "byte_stream.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ) {}

void Writer::push( string data )
{
  size_t sz = data.size();
  if(sz > capacity_ - buffer_.size()){
    sz = capacity_ - buffer_.size();
  }
  cnt_in_ += sz;
  for(size_t i = 0; i < sz; i++){
    buffer_.push(data[i]);
  }
}

void Writer::push( char data ) {
 if(buffer_.size() < capacity_){      // 只要还有空间就可以插入
    cnt_in_++;
    buffer_.push(data);
 }
}

void Writer::close()
{
  close_status_ = true;
}

void Writer::set_error()
{
  error_status_ = true;
}

bool Writer::is_closed() const
{
  return close_status_;
}

uint64_t Writer::available_capacity() const
{
  return capacity_ - buffer_.size();
}

uint64_t Writer::bytes_pushed() const
{
  return cnt_in_;
}

string_view Reader::peek() const
{
  return {&buffer_.front(), 1};
}

bool Reader::is_finished() const
{
  return close_status_ && buffer_.empty();
}

bool Reader::has_error() const
{
  return error_status_;
}

void Reader::pop( uint64_t len )
{
  auto sz = len;
  if(sz > buffer_.size()){
    sz = buffer_.size();
  }
  cnt_out_ += sz;
  while(sz--){
    buffer_.pop();
  }
}

uint64_t Reader::bytes_buffered() const
{
  return buffer_.size();
}

uint64_t Reader::bytes_popped() const
{
  return cnt_out_;
}
