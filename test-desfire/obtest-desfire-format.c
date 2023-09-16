// using libfreefare

// format card

int main
  (int argc,
  char *argv [])

{ /* main for obtest-desfire-format */

// select master application
  blah blah mifare_desfire_select_application(tag, NULL);

// get version info
  blah blah mifare_desfire_get_version(tag, &version_info);

// authenticate with aes key from settings

// format the card
  blah blah mifare_desfire_foramt_picc(tag);


  return(-1);

} /* main for obtest-desfire-format */

