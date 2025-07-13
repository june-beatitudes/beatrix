int bea_errno;

extern int *
__errno (void)
{
  return &bea_errno;
}
