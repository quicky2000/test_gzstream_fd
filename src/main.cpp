/*
    This file is part of test_gzstream_fd
    Copyright (C) 2013  Julien Thevenon ( julien_thevenon at yahoo.fr )

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#include "gzstream.h"
#include "url_reader.h"
#include "download_buffer.h"
#include <stdlib.h>
#include <stdio.h>

#ifndef _WIN32
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#else
#include "windows.h"
#include <Fcntl.h>
#endif

using namespace quicky_url_reader;

int main(void)
{
  // Obtain FIle descriptor
#ifdef _WIN32
  HANDLE l_file_handle = CreateFile("toto",GENERIC_READ | GENERIC_WRITE,0,0,OPEN_ALWAYS,/*FILE_ATTRIBUTE_NORMAL*/FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE,NULL);
  printf("l_file_handle = %p\n",l_file_handle);
  if(INVALID_HANDLE_VALUE == l_file_handle)
    {
      printf("ERROR when opening temporary file\n");
      exit(-1);
    }
  
//TO DELETE  HANDLE l_handle = CreateFileMapping(l_file_handle/*INVALID_HANDLE_VALUE*/,0,PAGE_READWRITE,0,l_download_buffer.get_size(),NULL);
//TO DELETE  if(INVALID_HANDLE_VALUE == l_handle)
//TO DELETE    {
//TO DELETE      printf("ERROR when opening file mapping\n");
//TO DELETE      exit(-1);
//TO DELETE    }
//TO DELETE  
//TO DELETE  LPVOID l_mapped_memory = MapViewOfFile(l_handle,FILE_MAP_WRITE,0,0,0);
//TO DELETE  if(l_mapped_memory == NULL)
//TO DELETE    {
//TO DELETE      printf("Error when mapping file in memory\n");
//TO DELETE      exit(-1);
//TO DELETE    }
//TO DELETE  memcpy(l_mapped_memory,l_buffer,l_download_buffer.get_size());
//TO DELETE  printf("l_handle = %d\n",l_handle);
  
  // Get the file descriptor from the handle
  int fd = _open_osfhandle((long)l_file_handle, O_RDONLY);
  printf("File descriptor = %d\n",fd);
#else
  int fd = shm_open("/sharedmem",O_RDWR|O_CREAT,0);
  if(fd<0) perror(NULL);
#endif
  std::string l_url = "http://download.openstreetmap.fr/replication/europe/france/minute/000/091/982.osc.gz";
  url_reader & l_url_reader = url_reader::instance();
  l_url_reader.dump_url(l_url,fd);

  lseek(fd,0,SEEK_SET);

  igzstream l_input_file(fd);
  while(!l_input_file.eof())
    {
      std::string l_line;
      getline(l_input_file,l_line);
      std::cout << l_line << std::endl ;
    }
  l_input_file.close();


#ifdef _WIN32
//TO DELETE  UnmapViewOfFile(l_mapped_memory);
//TO DELETE  CloseHandle(l_handle);
  CloseHandle(l_file_handle);
#else
		    //  fclose(l_file);
  shm_unlink("/sharedmem");
#endif

}

#if 0
  std::string l_url = "http://download.openstreetmap.fr/replication/europe/france/minute/000/091/892.osc.gz";
  url_reader & l_url_reader = url_reader::instance();
  download_buffer l_download_buffer;
  l_url_reader.read_url(l_url.c_str(),l_download_buffer);

  const char * l_buffer = l_download_buffer.get_data();
#ifdef _WIN32
  HANDLE l_file_handle = CreateFile("toto",GENERIC_READ | GENERIC_WRITE,0,0,OPEN_ALWAYS,/*FILE_ATTRIBUTE_NORMAL*/FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE,NULL);
  printf("l_file_handle = %d\n",l_file_handle);
  if(INVALID_HANDLE_VALUE == l_file_handle)
    {
      printf("ERROR when opening temporary file\n");
      exit(-1);
    }
  
  HANDLE l_handle = CreateFileMapping(l_file_handle/*INVALID_HANDLE_VALUE*/,0,PAGE_READWRITE,0,l_download_buffer.get_size(),NULL);
  if(INVALID_HANDLE_VALUE == l_handle)
    {
      printf("ERROR when opening file mapping\n");
      exit(-1);
    }
  
  LPVOID l_mapped_memory = MapViewOfFile(l_handle,FILE_MAP_WRITE,0,0,0);
  if(l_mapped_memory == NULL)
    {
      printf("Error when mapping file in memory\n");
      exit(-1);
    }
  memcpy(l_mapped_memory,l_buffer,l_download_buffer.get_size());
  printf("l_handle = %d\n",l_handle);
  
  // Get the file descriptor from the handle
  int fd = _open_osfhandle((long)l_file_handle, O_RDONLY);
  printf("File descriptor = %d\n",fd);
  
  // Get the handle from the file descriptor
  FILE * l_file = _fdopen(fd,"r");
#else
  //  FILE * l_file = fmemopen((void*)l_buffer,l_download_buffer.get_size(),"r");
  //  int fd = fileno(l_file);
  int fd = shm_open("/sharedmem",O_RDWR|O_CREAT,0);
  if(fd<0) perror(NULL);
  write(fd,l_buffer,l_download_buffer.get_size());
  lseek(fd,0,SEEK_SET);
#endif


#if 0
  std::string l_name = "../soda_batch/tmp_diff.gz";
  std::ifstream l_tmp_input_file(l_name.c_str());
  if(l_tmp_input_file==NULL)
    {
      std::cout << "ERROR : Unable to open file \"" << l_name << "\"" << std::endl ;
      exit(-1);
    }
  l_tmp_input_file.close();

  igzstream l_input_file(l_name.c_str());
#endif

  igzstream l_input_file(fd);
  while(!l_input_file.eof())
    {
      std::string l_line;
      getline(l_input_file,l_line);
      std::cout << l_line << std::endl ;
    }
  l_input_file.close();


#ifdef _WIN32
  UnmapViewOfFile(l_mapped_memory);
  CloseHandle(l_handle);
  CloseHandle(l_file_handle);
#else
		    //  fclose(l_file);
  shm_unlink("/sharedmem");
#endif

}
#endif
//EOF
