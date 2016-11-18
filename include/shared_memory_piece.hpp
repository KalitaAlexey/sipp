#ifndef SHARED_MEMORY_PIECE_H
#define SHARED_MEMORY_PIECE_H

class SharedMemoryPiece
{
public:
  SharedMemoryPiece(const char* const name, const int size);

  ~SharedMemoryPiece();

  SharedMemoryPiece(const SharedMemoryPiece& o);

  void* GetPtr();

  void OpenForReading();

  void OpenForWriting();

  void Close();

  void SetShouldCloseOnDestruction(const bool shouldCloseOnDestruction);
private:
  char* m_name;
  int m_size;
  int m_id;
  void* m_ptr;
  bool m_shouldCloseOnDestruction;

  void Open(const int flags);

  void Map(const int flags);
};

#endif
