oyOBJECT_e         oyCMMobjectType_GetId
                                     ( oyCMMobjectType_s * obj )
{
  return ((oyCMMobjectType_s_*) obj)->id;
}
const char *       oyCMMobjectType_GetPaths
                                     ( oyCMMobjectType_s * obj )
{
  return ((oyCMMobjectType_s_*) obj)->paths;
}
oyCMMobjectGetText_f
                   oyCMMobjectType_GetTextF
                                     ( oyCMMobjectType_s * obj )
{
  return ((oyCMMobjectType_s_*) obj)->oyCMMobjectGetText;
}
