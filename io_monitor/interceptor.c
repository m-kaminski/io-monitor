//*****************************************************************************


void check_for_http(int dom, int fd, const char* buf, size_t count, struct timeval *s, struct timeval *e)
{
  char buffer1[PATH_MAX];
  char buffer2[STR_LEN];

  int line = 0;
  int i;
  int linelen[2] = {0,0};
  char *tgt;

  for (i = 0; i!= count;  i++) {
    if (buf[i]==0)
      return; // not a HTTP header!

    if (buf[i]=='\r') {
      if (i<count && buf[i+1] == '\n') {
	i++;
	line++;
	if (line > 1)
	  break;
      } else {
	return; // not a HTTP!
      }
    }
    if (line) {
      buffer2[linelen[1]]=buf[i];
      linelen[1]++;
      if (linelen[1]>=STR_LEN)
	return;
      buffer2[linelen[1]]=0;
    } else {
      buffer1[linelen[0]]=buf[i];
      linelen[0]++;
      if (linelen[0]>=PATH_MAX)
	return;
      buffer1[linelen[0]]=0;
    }
  }
  if (!strstr(buffer1, "HTTP")) {
    return; // Not a HTTP event!
  }

  if ((!strncmp("GET ",buffer1, 4))
      || (!strncmp("PUT ", buffer1, 4))
      || (!strncmp("HEAD ", buffer1, 5))
      || (!strncmp("POST ", buffer1, 5))
      || (!strncmp("DELETE ", buffer1, 7))) {
    if (dom == FILE_WRITE) {
      record(HTTP, HTTP_REQ_SEND, fd, buffer1, buffer2,
	     s, e, 0, 0);
    } else {
      record(HTTP, HTTP_REQ_RECV, fd, buffer1, buffer2,
	     s, e, 0, 0);
    }
  }
  
}

