#include <random>
#include <memory>
#include <iostream>
#include <vector>
//bits/stdc++.h подключать нельзя, кажется

//using namespace писать тоже нельзя


class Treap {
private:
  typedef long long Int64;
  /*
   * Все функции, которые не работают с конкретным объектом Treap,
   * должны быть static
   */
  static const size_t kLeft = 0, kRight = 1;
  static size_t GetAnotherDirection(size_t);
  /*
   * Избавление от магических констант, показывающих направление.
   * Да, делать another_direction = direction ^ 1 в явном виде в коде нельзя,
   * надо подобные штуки через функцию.
   */
  static size_t GetRandomNumber();
  /*
   * Вроде можно генератор рандома обернуть в класс, но на это Ф.Д.
   * не особо смотрел.
   */
  struct Node {
    /*
     * Структура, потому что хранит данные и ничего не делает,
     * но вроде это тоже не важно.
     */
    Node *child[2]; // Чтобы одинаково всё делать для L и R
    size_t size;
    size_t priority;
    size_t no_increment_affix_length[2];//0 - префикс, 1 - суффикс
    size_t no_decrement_affix_length[2];
    Int64 value;
    Int64 new_value;
    Int64 sum;
    Int64 added_value;
    Int64 border_value[2];
    bool reverse_flag;
    bool assign_flag;
    explicit Node(Int64);
  };
  typedef Node *NodePtr;
  /*
   * Внутри класса такой typedef можно
   * ATTENTION!!! Не стоит делать typedef std::shared_ptr <Node>,
   * они долгие и получают TL
   */
  static void RemoveNode(NodePtr &);//Чтобы спрятать вызов delete
  template <typename ...args>//и new. Переменное число аргументов - чтобы не
  static void AllocNode(NodePtr &, args...);//пришлось чего менять при
  //изменении конструктора Node
  static size_t Size(NodePtr);
  static void Assign(NodePtr, Int64);
  static void Add(NodePtr, Int64);
  template <typename T>
  static void Swap(T *);//Оно сделало код короче
  static void Reverse(NodePtr);
  static void Push(NodePtr);
  static size_t PartialSize(NodePtr, size_t); // Левое или правое поддерево + 1
  template <typename Comparator>//Это просто для избавления от одной копипасты
  static void FirstStepOfAffixUpdating(NodePtr, NodePtr, size_t &, size_t,
                                       size_t, Comparator);
  template <typename Comparator>
  static void SecondStepOfAffixUpdating(NodePtr, NodePtr, size_t &, size_t,
                                 size_t, Comparator);
  static void Recalculate(NodePtr);//Не Recalc, сокращения слов - плохо
  static void Merge(NodePtr, NodePtr, NodePtr &);
  static void Split(NodePtr, NodePtr &, NodePtr &, size_t);
  static void MultiMerge(NodePtr, NodePtr, NodePtr, NodePtr &);
  static void GetSegment(NodePtr, NodePtr &, NodePtr &,//gets [l, r),
                         NodePtr &, size_t, size_t);  // l in [1..size)
  template <class Comparator>
  static size_t FindPositionOfNearestElement(
      NodePtr, Int64, Comparator);
  template <class Comparator>
  static void SimplePermute(NodePtr &, Comparator);
  //static void NextPermutationOnSegment(NodePtr &);
  //static void PrevPermutationOnSegment(NodePtr &);
  enum class PermutationType : bool { kNextPermutation, kPrevPermutation };
  template <PermutationType flag>
  static void PermutationOnSegment(NodePtr &);
  template <typename ReturnType, class Function>//Делает Function на отрезке
  static ReturnType DoOperation(NodePtr &, size_t, size_t, Function);
  template <class Function>
  static void DfsInOrder(NodePtr, Function);
  template <class Function>
  static void DfsPostOrder(NodePtr, Function);
  class Subsegment { ///Этот класс позволяет работать с подотрезком,
  private:/// не заботясь о том, будет ли он вставлен обратно
    NodePtr &all_segment_;///Т.е. это просто обёртка вокруг GetSegment()
    NodePtr left_, target_subsegment_, right_;
  public:
    explicit Subsegment(NodePtr &, size_t, size_t);
    ~Subsegment();
    operator NodePtr &();
    //explicit operator NodePtr(); ///Во избежание неоднозначности
    Node &operator*();
    NodePtr operator->();
  };
  NodePtr root_;
public:
  size_t size() const;
  explicit Treap();
  ~Treap();
  template <typename IteratorType>
  explicit Treap(IteratorType, IteratorType);
  template <typename IteratorType>
  void GetArray(IteratorType);//Не важно, куда выводить дерево
  Int64 Sum(size_t, size_t);
  void Insert(Int64, size_t);
  void Erase(size_t);
  void Assign(Int64, size_t, size_t);
  void Add(Int64, size_t, size_t);
  void NextPermutation(size_t, size_t);
  void PrevPermutation(size_t, size_t);
};

Treap::Node::Node(Int64 value)
  : size(1),
    priority(GetRandomNumber()),
    value(value), new_value(),
    sum(value), added_value(0),
    reverse_flag(false), assign_flag(false) {
  for (size_t index : { kLeft, kRight }) {
    child[index] = nullptr;
    no_increment_affix_length[index] = 1;
    no_decrement_affix_length[index] = 1;
    border_value[index] = value;
  }
}

size_t Treap::Size(NodePtr node) {
  return (node == nullptr)
      ? 0
      : node->size;
}

void Treap::Assign(NodePtr node, Int64 value) {
  node->value = node->new_value = value;
  node->added_value = 0;
  node->sum = node->size * value;
  for (auto direction : { kLeft, kRight }) {
    node->border_value[direction] = value;
    node->no_decrement_affix_length[direction] = node->size;
    node->no_increment_affix_length[direction] = node->size;
  }
  node->assign_flag = true;
}

void Treap::Add(NodePtr node, Int64 value) {
  node->value += value;
  node->added_value += value;
  node->sum += node->size * value;
  for (auto direction : { kLeft, kRight })
    node->border_value[direction] += value;
}

template<typename T>
void Treap::Swap(T* array_of_two_elements)
{
  std::swap(array_of_two_elements[kLeft], array_of_two_elements[kRight]);
}

void Treap::Reverse(NodePtr node) {
  Swap(node->child);
  Swap(node->no_decrement_affix_length);
  Swap(node->no_increment_affix_length);
  Swap(node->border_value);
  node->reverse_flag ^= true;
}

void Treap::Push(NodePtr node) {
  for (auto child : node->child)
    if (child) {
      if (node->assign_flag)
        Assign(child, node->new_value);
      Add(child, node->added_value);
      if (node->reverse_flag)
        Reverse(child);
    }
  node->assign_flag = node->reverse_flag = false;
  node->added_value = 0;
}

size_t Treap::GetAnotherDirection(size_t direction) {
  return (direction == kLeft)
      ? kRight
      : kLeft;
}

size_t Treap::GetRandomNumber()
{
  static std::mt19937 generator;
  static std::uniform_int_distribution <size_t> distributor;
  return distributor(generator);
}

void Treap::RemoveNode(Treap::NodePtr &pointer)
{
  delete pointer;
  pointer = nullptr;
}

template <typename ...args>
void Treap::AllocNode(Treap::NodePtr &pointer, args ...arg_list)
{
  pointer = new Node(arg_list...);
}

size_t Treap::PartialSize(NodePtr node, size_t direction) {
  return 1 + Size(node->child[direction]);
}

template <typename Comparator>
void Treap::FirstStepOfAffixUpdating(
    NodePtr updating_node, NodePtr first_child,
    size_t &affix_length_of_node, size_t affix_length_of_child,
    size_t direction, Comparator compare) {
  affix_length_of_node = affix_length_of_child;
  if (affix_length_of_node == Size(first_child) && compare(
        updating_node->value,
        first_child->border_value[GetAnotherDirection(direction)]))
    ++affix_length_of_node;
}

template <typename Comparator>
void Treap::SecondStepOfAffixUpdating(
    NodePtr updating_node, NodePtr second_child,
    size_t &affix_length_of_node, size_t affix_length_of_child,
    size_t direction, Comparator compare) {
  if (affix_length_of_node
          == PartialSize(updating_node, direction)
          && compare(second_child->border_value[direction],
                     updating_node->value))
        affix_length_of_node += affix_length_of_child;
}

void Treap::Recalculate(NodePtr node)
{
  node->sum = node->value;
  node->size = 1;
  NodePtr child;
  size_t another_direction;
  for (auto direction : { kLeft, kRight })
    if (node->child[direction]) {
      child = node->child[direction];
      node->sum += child->sum;
      node->size += Size(child);
      node->border_value[direction] = child->border_value[direction];
      FirstStepOfAffixUpdating(
            node, child, node->no_decrement_affix_length[direction],
            child->no_decrement_affix_length[direction], direction,
            [](Int64 node_value, Int64 border_value)->bool {
        return node_value <= border_value; });
      FirstStepOfAffixUpdating(
            node, child, node->no_increment_affix_length[direction],
            child->no_increment_affix_length[direction], direction,
            [](Int64 node_value, Int64 border_value)->bool {
        return node_value >= border_value; });//lambda-компаратор, ага.
    } else {
      node->border_value[direction] = node->value;
      node->no_decrement_affix_length[direction] = 1;
      node->no_increment_affix_length[direction] = 1;
    }
  for (auto direction : { kLeft, kRight }) {
    another_direction = GetAnotherDirection(direction);
    child = node->child[another_direction];
    if (!child)
      continue;
    SecondStepOfAffixUpdating(node, child,
                              node->no_decrement_affix_length[direction],
                              child->no_decrement_affix_length[direction],
                              direction,
                              [](Int64 border_value, Int64 node_value)->bool {
      return border_value <= node_value;
    });
    SecondStepOfAffixUpdating(node, child,
                              node->no_increment_affix_length[direction],
                              child->no_increment_affix_length[direction],
                              direction,
                              [](Int64 border_value, Int64 node_value)->bool {
      return border_value >= node_value;
    });
  }
}

void Treap::Merge(NodePtr left_node, NodePtr right_node, NodePtr &result) {
  if (!left_node || !right_node) {
    result = (left_node)
        ? left_node
        : right_node;
    return;
  }
  Push(left_node);
  Push(right_node);
  if (left_node->priority > right_node->priority) {
    result = left_node;
    Merge(left_node->child[kRight], right_node, left_node->child[kRight]);
  } else {
    result = right_node;
    Merge(left_node, right_node->child[kLeft], right_node->child[kLeft]);
  }
  Recalculate(result);
}

void Treap::Split(NodePtr segment, NodePtr &left_part,
                  NodePtr &right_part, size_t left_size) {
  if (!segment) {
    left_part = right_part = nullptr;
    return;
  }
  Push(segment);
  if (PartialSize(segment, kLeft) <= left_size) {
    left_part = segment;
    Split(segment->child[kRight], segment->child[kRight],
          right_part, left_size - PartialSize(segment, kLeft));
  } else {
    right_part = segment;
    Split(segment->child[kLeft], left_part,
          segment->child[kLeft], left_size);
  }
  Recalculate(segment);
}

void Treap::MultiMerge(NodePtr left_part, NodePtr middle_part,
                       NodePtr right_part, NodePtr &segment) {
  Merge(left_part, middle_part, left_part);
  Merge(left_part, right_part, segment);
}

void Treap::GetSegment(NodePtr segment, NodePtr &left_part,
                       NodePtr &middle_part, NodePtr &right_part,
                       size_t left_border, size_t right_border) {
  Split(segment, segment, right_part, right_border - 1);
  Split(segment, left_part, middle_part, left_border - 1);
}

template <Treap::PermutationType flag>
void Treap::PermutationOnSegment(NodePtr &segment) {
  size_t suffix_length = (flag == PermutationType::kNextPermutation)
      ? segment->no_increment_affix_length[kRight]
      : segment->no_decrement_affix_length[kRight];
  if (suffix_length == Size(segment)) {
    Reverse(segment);
    return;
  }
  NodePtr prefix, suffix;
  Split(segment, prefix, suffix,
        Size(segment) - suffix_length - 1);
  SimplePermute(suffix, (flag == PermutationType::kNextPermutation)
                ? [](Int64 a, Int64 b)->bool {
                  return a > b;
                }
                : [](Int64 a, Int64 b)->bool {
                  return a < b;
  });
  Merge(prefix, suffix, segment);
}

size_t Treap::size() const
{
  return Size(root_);
}

template<class Comparator>
size_t Treap::FindPositionOfNearestElement(Treap::NodePtr node,
                                           Int64 target_value,
                                           Comparator comparator) {
  Push(node);
  if (node->child[kRight]
      && comparator(node->child[kRight]->border_value[kLeft], target_value))
    return PartialSize(node, kLeft) + FindPositionOfNearestElement(
          node->child[kRight], target_value, comparator);
  if (comparator(node->value, target_value))
    return PartialSize(node, kLeft);
  return FindPositionOfNearestElement(node->child[kLeft],
                                      target_value, comparator);
}


template <class Comparator>
void Treap::SimplePermute(NodePtr &segment, Comparator comparator) {
  NodePtr first_element, sorted_suffix;
  Split(segment, first_element, sorted_suffix, 1);
  NodePtr left_part, element_to_swap, right_part;
  size_t position_of_nearest_element = FindPositionOfNearestElement(
        sorted_suffix, first_element->value, comparator);
  GetSegment(sorted_suffix, left_part, element_to_swap, right_part,//[pos,pos+1)
             position_of_nearest_element, position_of_nearest_element + 1);
  std::swap(first_element, element_to_swap);
  MultiMerge(left_part, element_to_swap, right_part, sorted_suffix);
  Reverse(sorted_suffix);
  Merge(first_element, sorted_suffix, segment);
}

template<typename ReturnType, class Function>
ReturnType Treap::DoOperation(Treap::NodePtr &node, size_t left_index,
                              size_t right_index, Function operation) {
  return operation(Subsegment(node, left_index + 1, right_index + 1));
}

template <class Function>
void Treap::DfsInOrder(NodePtr node, Function f)
{
  if (!node)
    return;
  Push(node);
  DfsInOrder(node->child[kLeft], f);
  f(node);
  DfsInOrder(node->child[kRight], f);
}

Treap::Treap() : root_(nullptr) {}

Treap::~Treap() {
  DfsPostOrder(root_, [](NodePtr v) {
    delete v;
  });
}

Treap::Int64 Treap::Sum(size_t left_index, size_t right_index) {
  return DoOperation<Int64>(root_, left_index, right_index + 1,
                            [](NodePtr &node)->Int64 { return node->sum; });
}

void Treap::Insert(Treap::Int64 value, size_t position) {
  DoOperation<void>(root_, position, position,
                    [value](NodePtr &new_node)->void {
    AllocNode(new_node, value);
  });
}

void Treap::Erase(size_t position) {
  DoOperation<void>(root_, position, position + 1,
                    [](NodePtr &node_to_delete)->void {
    RemoveNode(node_to_delete);
  });
}

template <typename IteratorType>
Treap::Treap(IteratorType begin, IteratorType end) :root_(nullptr) {
  while (begin != end) {
    NodePtr new_node;
    AllocNode(new_node, *(begin++));
    Merge(root_, new_node, root_);
  }
}

template <typename IteratorType>
void Treap::GetArray(IteratorType begin) {
  DfsInOrder(root_,
          [&begin](NodePtr node)->void {
    *(begin++) = node->value;
  });
}

void Treap::NextPermutation(size_t left_index, size_t right_index) {
  DoOperation<void>(root_, left_index, right_index + 1,
                    PermutationOnSegment<PermutationType::kNextPermutation>);
}

void Treap::PrevPermutation(size_t left_index, size_t right_index) {
  DoOperation<void>(root_, left_index, right_index + 1,
                    PermutationOnSegment<PermutationType::kPrevPermutation>);
}

void Treap::Assign(Int64 value, size_t left_index, size_t right_index) {
  DoOperation<void>(root_, left_index, right_index + 1,
                    [value](NodePtr &segment) {
    Assign(segment, value);
  });
}

void Treap::Add(Int64 value, size_t left_index, size_t right_index) {
  DoOperation<void>(root_, left_index, right_index + 1,
                    [value](NodePtr &segment) {
    Add(segment, value);
  });
}

Treap::Subsegment::Subsegment(NodePtr &segment, size_t left_index,
                              size_t right_index)
  : all_segment_(segment), left_(nullptr), target_subsegment_(nullptr),
  right_(nullptr) {
  GetSegment(segment, left_, target_subsegment_,
             right_, left_index,right_index);
}

Treap::Subsegment::~Subsegment() {
  MultiMerge(left_, target_subsegment_, right_, all_segment_);
}

Treap::Node &Treap::Subsegment::operator*() {
  return *target_subsegment_;
}

Treap::Subsegment::operator NodePtr&() {
  return target_subsegment_;
}

Treap::NodePtr Treap::Subsegment::operator->() {
  return target_subsegment_;
}


template<class Function>
void Treap::DfsPostOrder(Treap::NodePtr node, Function f) {
  if (node == nullptr)
    return;
  for (auto next_node : node->child)
    DfsPostOrder(next_node, f);
  f(node);
}

struct Query {
  enum Type { SUM = 1, INSERT, ERASE, ASSIGN, ADD,
              NEXT_PERMUTATION, PREV_PERMUTATION };
  virtual void Read(std::istream &) = 0;
  virtual Type getType() const = 0;
  virtual ~Query() {}
};

struct PureSegmentQuery : Query {
  Type query_type;
  size_t left_border, right_border;
  PureSegmentQuery(Type type);
  void Read(std::istream &);
  Type getType() const;
};

Query::Type PureSegmentQuery::getType() const { return query_type; }

PureSegmentQuery::PureSegmentQuery(Type type) : query_type(type) {}

void PureSegmentQuery::Read(std::istream &in) {
  in >> left_border >> right_border;
}

struct SegmentModifyQuery : PureSegmentQuery {
  int value;
  SegmentModifyQuery(Type type);
  void Read(std::istream &);
};

SegmentModifyQuery::SegmentModifyQuery(Type type) : PureSegmentQuery(type) {}

void SegmentModifyQuery::Read(std::istream &in) {
  in >> value;
  ::PureSegmentQuery::Read(in);
}

struct PureElementQuery : Query {
  Type query_type;
  size_t position;
  PureElementQuery(Type type);
  void Read(std::istream &);
  Type getType() const;
};

Query::Type PureElementQuery::getType() const { return query_type; }

PureElementQuery::PureElementQuery(Type type) : query_type(type) {}

void PureElementQuery::Read(std::istream &in) {
  in >> position;
}

struct ElementModifyQuery : PureElementQuery {
  int value;
  ElementModifyQuery(Type type);
  void Read(std::istream &);
};

ElementModifyQuery::ElementModifyQuery(Type type) : PureElementQuery(type) {}

void ElementModifyQuery::Read(std::istream &in) {
  in >> value;
  PureElementQuery::Read(in);
}

Query *ReadQuery(std::istream &);

Query *ReadQuery(std::istream &input_stream)
{
  int just_an_integer;
  input_stream >> just_an_integer;
  Query::Type type_of_this_query = static_cast<Query::Type>(just_an_integer);
  Query *read_query = nullptr;
  switch (type_of_this_query) {
  case Query::SUM:
    read_query = new PureSegmentQuery(type_of_this_query); break;
  case Query::INSERT:
    read_query = new ElementModifyQuery(type_of_this_query); break;
  case Query::ERASE:
    read_query = new PureElementQuery(type_of_this_query); break;
  case Query::ASSIGN:
    read_query = new SegmentModifyQuery(type_of_this_query); break;
  case Query::ADD:
    read_query = new SegmentModifyQuery(type_of_this_query); break;
  case Query::NEXT_PERMUTATION:
    read_query = new PureSegmentQuery(type_of_this_query); break;
  case Query::PREV_PERMUTATION:
    read_query = new PureSegmentQuery(type_of_this_query); break;
  }
  read_query->Read(input_stream);
  return read_query;
}

std::vector <Query*> ReadInput(std::istream &);
std::vector <int> ReadArray(std::istream &);

std::vector <int> ReadArray(std::istream &input_stream) {
  size_t number_of_numbers;
  input_stream >> number_of_numbers;
  std::vector <int> array(number_of_numbers);
  for (int &read_to : array)
    input_stream >> read_to;
  return array;
}

std::vector <Query*> ReadQueries(std::istream &input_stream) {
  size_t number_of_queries;
  input_stream >> number_of_queries;
  std::vector <Query*> input_data(number_of_queries);
  for (auto &query : input_data)
    query = ReadQuery(input_stream);
  return input_data;
}

template <class Array, typename IteratorType>
inline void Process(Query *query, Array &container,
                    IteratorType output_iterator) {
  switch (query->getType()) {
  case Query::SUM:
    *(output_iterator++) = container.Sum(
          dynamic_cast<PureSegmentQuery*>(query)->left_border,
          dynamic_cast<PureSegmentQuery*>(query)->right_border);
    return;
  case Query::INSERT:
    container.Insert(
          dynamic_cast<ElementModifyQuery*>(query)->value,
          dynamic_cast<ElementModifyQuery*>(query)->position);
    return;
  case Query::ERASE:
    container.Erase(dynamic_cast<PureElementQuery*>(query)->position);
    return;
  case Query::ASSIGN:
    container.Assign(
          dynamic_cast<SegmentModifyQuery*>(query)->value,
          dynamic_cast<SegmentModifyQuery*>(query)->left_border,
          dynamic_cast<SegmentModifyQuery*>(query)->right_border);
    return;
  case Query::ADD:
    container.Add(
          dynamic_cast<SegmentModifyQuery*>(query)->value,
          dynamic_cast<SegmentModifyQuery*>(query)->left_border,
          dynamic_cast<SegmentModifyQuery*>(query)->right_border);
    return;
  case Query::NEXT_PERMUTATION:
    container.NextPermutation(
          dynamic_cast<PureSegmentQuery*>(query)->left_border,
          dynamic_cast<PureSegmentQuery*>(query)->right_border);
    return;
  case Query::PREV_PERMUTATION:
    container.PrevPermutation(
          dynamic_cast<PureSegmentQuery*>(query)->left_border,
          dynamic_cast<PureSegmentQuery*>(query)->right_border);
    return;
  }
}

template <typename T>
void DeletePointers(std::vector <T *> &v)
{
  while (!v.empty())
  {
    delete v.back();
    v.pop_back();
  }
}
//Надо разделять ввод, вывод, решение
struct InputData
{
  std::vector <int> array;
  std::vector <Query *> queries;
  ~InputData()
  {
    for (size_t i(0); i < queries.size(); ++i)
      delete queries[i];
  }
};
//Возвращает структурку векторов, тут всё ОК.
InputData GlobalRead(std::istream &in)
{//Ну RVO, за подробностями наерное к Мещерину
  InputData result;
  result.array = ReadArray(in);
  result.queries = ReadQueries(in);
  return result;
}

/*
 * Если кратко:
 * Если переменная создана внутри функции и возвращается функцей,
 * то умный компилятор создаёт эту переменную ровно там, куда она будет
 * возвращена. Иными словами это эквивалентно такому коду:
 * InputData input;
 * GlobalRead(InputData &...
 */

struct OutputData
{
  std::vector <long long> answers, array;
};

void GlobalWrite(const OutputData &output, std::ostream &out)
{
  for (auto number : output.answers)
    out << number << '\n';
  for (auto number : output.array)
    out << number << ' ';
}

template <class Array>
OutputData Solution(const InputData &input) {
  OutputData output;
  Array container(std::begin(input.array), std::end(input.array));
  for (Query *query : input.queries)//А вот тут очень удобные итераторы
    Process(query, container, std::back_inserter(output.answers));
  container.GetArray(std::back_inserter(output.array));
  return output;
}

/*
 * В функции решения(здесь - GlobalSolution) должны быть:
 * 1) Считывание данных
 * 2) Обработка(запуск решения задачи)
 * 3) Вывод данных
 * Т.е. вызов трёх функций
 */
template <class Array>
void GlobalSolution(std::istream &in, std::ostream &out)
{//А, ещё. Важно, чтобы не через cin всё. Т.е. чтоб с любыми потоками можно.
  GlobalWrite(Solution<Array>(GlobalRead(in)), out);
}

int main() {
  //freopen("input.txt", "r", stdin);
  std::ios_base::sync_with_stdio(false);
  GlobalSolution <Treap>(std::cin, std::cout);
}
