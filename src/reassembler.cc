#include "reassembler.hh"

using namespace std;


void Reassembler::merge( Pending_type t )
{
  if(pending_buffer.empty()){     // 如果没有缓存数据可以直接合并
    pending_buffer[t.first_index] = t;
    return;
  }
  auto t_f = t.first_index;
  auto t_l = t.first_index + t.data.size();

  auto latter = pending_buffer.lower_bound(t.first_index);
  // 可以分为三种情况分别进行设计
  if(latter == pending_buffer.begin()){
    if(t_l <= latter->first){     // 没有重叠可以直接插入
      pending_buffer[t.first_index] = t;
      return;
    }         // 处理重叠的情况
    t.data = t.data.substr(0, latter->first - t_f);
    pending_buffer[t.first_index] = t;
  }else if(latter == pending_buffer.end()){
    
  }else{

  }
  auto prior = prev(latter);



}

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
    while(!pending_buffer.empty()){
      const Pending_type i = pending_buffer.begin()->second;
      if(i.first_index > next_index_ && data.size() < output.available_capacity()){   // 防止无限循环
        break;
      }
      pending_buffer.erase(i.first_index);
      cnt_pending_ -= i.data.size();
      insert(i.first_index, i.data, i.is_last_substring, output);
    }
  }else if(first_index > next_index_ && data.size() < output.available_capacity()){                                 // 缓存条件
      merge(Pending_type{first_index, data, is_last_substring});                // 合并到缓存中
      cnt_pending_ += data.size();
  }

  if(is_last_substring && pending_buffer.empty()){         // 当这是最后一个元素，内部缓存空间里面没有元素的时候关闭连接
    output.close();
  }
}

uint64_t Reassembler::bytes_pending() const
{
  return cnt_pending_;
}

