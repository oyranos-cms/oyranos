BEGIN{ p = 0 }
{
  if ($2 == "Function" && $3 == "__FUNC__")
    p = 1;
  if (p == 1) print $0;
  if ($0 == "}")
    p = 0
}
