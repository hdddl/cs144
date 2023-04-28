#include "reassembler.hh"

using namespace std;

/*
 * 1. 确定写入条件
 * 2. 确定缓存条件
 * 3. 缓存数据如何重新写入
 * 4. 重叠数据应该如何处理，采用什么数据结构
 */
void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring, Writer& output )
{
  if(output.available_capacity() == 0){
    return;
  }
  const uint64_t last_index = first_index + data.size();
  if(next_index_ >= first_index && next_index_ <= last_index){  // 写入条件，next 位于 first 与 last 之前，并且有容量可以写入
    uint64_t sz = last_index - next_index_;                    // 需要写入包的数量，如果写入的大于容量以容量为准，其他舍去
    if(sz > output.available_capacity()){
      sz = output.available_capacity();
    }
    output.push(data.substr(next_index_ - first_index, sz));
    next_index_ += sz;                                        // 更新 next_index
    while(!pending_buffer.empty()){                           // 处理缓存空间
      const uint64_t i = pending_buffer.begin()->first;
      const bool eof = pending_buffer.begin()->second.second;
      const char v = pending_buffer.begin()->second.first;
      if(i > next_index_ || output.available_capacity() == 0){
        break;
      }
      if(i == next_index_){
        pending_buffer.erase(i);
        cnt_pending_--;
        next_index_++;
        output.push(v);
        if(eof){
          is_last_substring = true;
        }
      }else if(i < next_index_){        // 重复的直接抹去
        cnt_pending_--;
        pending_buffer.erase(i);
      }
    }
  }else if(first_index > next_index_ && data.size() < output.available_capacity()){                                 // 缓存条件
    const uint64_t sz = data.size();
    for(uint64_t i = 0; i < sz; i++){
      if(!pending_buffer.contains(first_index+i)){
        pending_buffer[first_index + i] = pair<char, bool>{data[i], false};
        cnt_pending_++;
      }
    }
    if(is_last_substring){        // 标记为 EOF
      pending_buffer[first_index + sz - 1].second = true;
    }
  }

  if(is_last_substring && pending_buffer.empty()){         // 当这是最后一个元素，内部缓存空间里面没有元素的时候关闭连接
    output.close();
  }
}

uint64_t Reassembler::bytes_pending() const
{
  return cnt_pending_;
}

