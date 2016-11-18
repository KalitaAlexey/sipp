#include <cstring>
#include <cstdlib>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "shared_memory_piece.hpp"

SharedMemoryPiece::SharedMemoryPiece(const char* const name, const int size)
  : m_name(strdup(name))
  , m_size(size)
  , m_id(-1)
  , m_ptr(NULL)
  , m_shouldCloseOnDestruction(true)
{
}

SharedMemoryPiece::~SharedMemoryPiece()
{
  if (m_shouldCloseOnDestruction)
  {
    Close();
  }

  if (m_name != NULL)
  {
    free(m_name);
  }
}

SharedMemoryPiece::SharedMemoryPiece(const SharedMemoryPiece& o)
  : m_name(strdup(o.m_name))
  , m_size(o.m_size)
  , m_id(o.m_id)
  , m_ptr(o.m_ptr)
  , m_shouldCloseOnDestruction(o.m_shouldCloseOnDestruction)
{
}

void* SharedMemoryPiece::GetPtr()
{
  return m_ptr;
}

void SharedMemoryPiece::OpenForReading()
{
  Open(O_RDONLY);

  if (m_id == -1)
  {
    return;
  }

  Map(PROT_READ);
}

void SharedMemoryPiece::OpenForWriting()
{
  Open(O_RDWR | O_CREAT);

  if (m_id == -1)
  {
    return;
  }

  {
    const int res = ftruncate(m_id, m_size);

    if (res != 0)
    {
      return;
    }
  }

  Map(PROT_READ | PROT_WRITE);
}

void SharedMemoryPiece::Close()
{
  if (m_ptr != NULL)
  {
    munmap(m_ptr, m_size);
    m_ptr = NULL;
  }

  shm_unlink(m_name);
  m_id = -1;
}

void SharedMemoryPiece::SetShouldCloseOnDestruction(const bool shouldCloseOnDestruction)
{
  m_shouldCloseOnDestruction = shouldCloseOnDestruction;
}

void SharedMemoryPiece::Open(const int flags)
{
  m_id = shm_open(m_name, flags, 0666);
}

void SharedMemoryPiece::Map(const int flags)
{
  m_ptr = mmap(0, m_size, flags, MAP_SHARED, m_id, 0);

  if (m_ptr == MAP_FAILED)
  {
    m_ptr = NULL;
  }

  if (m_ptr == NULL)
  {
    return;
  }
}
