BEGIN { doxy = 0; body = 0; save = 0; found = 0; buffer = "" }

function openbrace()
{
  if (body == 0 && found == 0) {
    save = 0
    buffer = ""
  }
  body++
}

function closebrace()
{
  if (body == 1 && save == 1) {
    buffer = buffer "\n" $0
    exit 0
  }
  body--
}

function opencomment()
{
  if (save && body == 0)
    buffer = ""
  if (body == 0)
    save = 1
  doxy = 1
}

/\/\*+/        { opencomment() }
/\*+\//        { doxy = 0 }
/^{ *$/        { if (doxy == 0) openbrace() }
/^} *$/        { if (doxy == 0) closebrace() }
/\<__FUNC__\>/ { if (doxy == 0 && body == 0 && save == 1) found = 1 }

{
  #print "d"doxy"b"body"s"save"f"found":", $0
  if (save) {
    if (buffer == "")
      buffer = $0
    else
      buffer = buffer "\n" $0
  }
}

END { if (found) print buffer }
