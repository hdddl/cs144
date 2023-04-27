#pragma once

#include "byte_stream.hh"

#include <queue>
#include <stack>
#include <string>
#include <unordered_map>

struct Pending_type{
  uint64_t first_index;
  std::string data;
  bool is_last_substring;
};

struct Pending_cmp{
  bool operator()(const Pending_type& t1, const Pending_type& t2)const{
    return t1.first_index > t2.first_index;
  }
};



class Reassembler
{
private:
  uint64_t next_index_ = 0;
  std::priority_queue<Pending_type, std::vector<Pending_type>, Pending_cmp>pending_data{};
  //std::unordered_map<size_t, std::string>data_;                                   // 用于存放未装载的数据
  uint64_t cnt_pending_ = 0;
public:
  /*
   * Insert a new substring to be reassembled into a ByteStream.
   *   `first_index`: the index of the first byte of the substring
   *   `data`: the substring itself
   *   `is_last_substring`: this substring represents the end of the stream
   *   `output`: a mutable reference to the Writer
   *
   * The Reassembler's job is to reassemble the indexed substrings (possibly out-of-order
   * and possibly overlapping) back into the original ByteStream. As soon as the Reassembler
   * learns the next byte in the stream, it should write it to the output.
   *
   * If the Reassembler learns about bytes that fit within the stream's available capacity
   * but can't yet be written (because earlier bytes remain unknown), it should store them
   * internally until the gaps are filled in.
   *
   * The Reassembler should discard any bytes that lie beyond the stream's available capacity
   * (i.e., bytes that couldn't be written even if earlier gaps get filled in).
   *
   * The Reassembler should close the stream after writing the last byte.
   */
  void insert( uint64_t first_index, std::string data, bool is_last_substring, Writer& output );

  // How many bytes are stored in the Reassembler itself?
  uint64_t bytes_pending() const;
};
