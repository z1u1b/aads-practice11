#include <iostream>

template < class T >
struct BiTree
{
  T val;
  BiTree< T >*lt, *rt, *parent;
};

// template< class T >
// struct BiTreeIt {
//   BiTree< T > * curr;
// };
enum Dir { fall_left, parent };
template < class T >
std::pair< size_t, BiTree< T >* > fallLeft(BiTree< T >* root)
{
  size_t path = 0;
  while (root->lt) {
    root = root->lt;
    path++;
  }
  return {path, root};
}

template < class T >
std::pair< size_t, BiTree< T >* > parent(BiTree< T >* root)
{
  if (!root) {
    return
    {
      0, nullptr
    }
  }
  size_t path = 0;
  BiTree< T >* parent = root->parent;
  while (parent && parent->lt != root) {
    root = parent;
    parent = root->parent;
    path++;
  }
  return {path, parent};
}

template < class T >
std::tuple< Dir, size_t, BiTree< T >* > nextStruct(BiTree< T >* root)
{
  if (root->rt) {
    auto result = fallLeft(root->rt);
    return {Dir::fall_left, result.first, result.second};
  }
  auto result = parent(root);
  return {Dir::parent, result.first, result.second};
}

template < class T >
std::tuple< BiTree< T >*, BiTree< T >*, bool > isEqualStructStart(BiTree< T >* lhs_start, BiTree< T >* rhs_start)
{
  auto ln = nextStruct(lhs_start);
  auto rn = nextStruct(rhs_start);
  while (std::get< 0 >(ln) == std::get< 0 >(rn) && std::get< 1 >(ln) == std::get< 1 >(rn) && std::get< 2 >(ln) &&
         std::get< 2 >(rn)) {
    ln = nextStruct(std::get< 2 >(ln));
    rn = nextStruct(std::get< 2 >(rn));
  }
  bool dir = std::get< 0 >(ln) == std::get< 0 >(rn);
  return {std::get< 2 >(ln), std::get< 2 >(rn), dir};
}

template < class T >
bool includedStruct(BiTree< T >* lhs, BiTree< T >* pattern)
{
  BiTree< T >* curr = fallLeft(lhs).second;
  while (curr) {
    auto result = isEqualStructStart(lhs, fallLeft(pattern).second));
    if (!std::get< 1 >(result) && std::get< 2 >(result)) {
      return true;
    }
    curr = nextStruct(curr).second;
  }
  return false;
}

template < class T >
bool isEqualStruct(BiTree< T >* lhs, BiTree< T >* rhs)
{
  auto lhs_begin = fallLeft(lhs);
  auto rhs_begin = fallLeft(rhs);
  if (lhs_begin.first != rhs_begin.first) {
    return false;
  }
  auto result = isEqualStructStart(lhs_begin.second, rhs_begin.second);
  return !std::get< 0 >(result) && !std::get< 1 >(result);
}

template < class T >
std::pair< BiTree< T >*, BiTree< T >* > // <- диапазон из lhs
inclusion(BiTree< T >* lhs,             // здесь ищем вхождения
          BiTree< T >* pattern          // образец структуры
)
{
  BiTree< T >* lhs_curr = fallLeft(lhs).second;
  while (lhs_curr) {
    auto result = isEqualStructStart(lhs_curr, fallLeft(pattern).second));
    if (!std::get< 1 >(result) && std::get< 2 >(result)) {
      BiTree< T >* last_lhs_next = std::get< 0 >(result);
      if (!last_lhs_next) {
        // lhs тоже закончился, нам нужен последний элемент
        BiTree< T >* lhs_end = lhs;
        while (lhs_end->rt) {
          lhs_end = lhs_end->rt;
        }
        return {lhs_curr, lhs_end};
      }
      BiTree< T >* lhs_end = prev(last_lhs_next);
      return {lhs_curr, lhs_end};
    }
    lhs_curr = nextStruct(lhs_curr).second;
  }
  return false;
}

template < class T >
BiTree< T >* prev(BiTree< T >* curr)
{
  BiTree< T >* next = curr;
  if (next->lt) {
    next = next->lt;
    while (next->rt) {
      next = next->tr;
    }
  } else {
    BiTree< T >* parent = next->parent;
    while (parent && parent->rt != next) {
      next = parent;
      parent = next->parent;
    }
    next = parent;
  }
  return next;
}

template < class T >
struct InclusionIt
{
  std::pair< BiTree< T >*, BiTree< T >* > incl;
};


template < class T >
InclusionIt< T > begin(BiTree< T >* lhs, BiTree< T >* pattern)
{
  InclusionIt< T > it;
  it.incl = inclusion(lhs, pattern);
  return it;
}

template < class T >
InclusionIt< T > next(InclusionIt< T > curr, BiTree< T >* pattern)
{
  if (!curr.incl.first || !curr.incl.second) {
    curr.incl = {nullptr, nullptr};
    return curr;
  }
  BiTree< T >* next_start = nextStruct(curr.incl.second).second;
  if (!next_start) {
    curr.incl = {nullptr, nullptr};
    return curr;
  }
  curr.incl = inclusion(next_start, pattern);
  return curr;
}

template < class T >
bool hasNext(InclusionIt< T > curr, BiTree< T >* pattern)
{
  return curr.incl.first != nullptr;
}

  

    