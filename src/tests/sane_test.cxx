#include <sane/sane.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
  int driver_version = 0;
  int status = sane_init(&driver_version, NULL);

  if (status == SANE_STATUS_GOOD)
         printf("SANE v%d.%d.%d\n",
                SANE_VERSION_MAJOR(driver_version),
                SANE_VERSION_MINOR(driver_version),
                SANE_VERSION_BUILD(driver_version));
  sane_exit();

#ifdef SANE_CAP_COLOR
  printf("SANE_CAP_COLOR");
#else
  printf("!!!Non color manageable SANE: no SANE_CAP_COLOR defined!!!");
  return 1;
#endif
  return 0;
}
