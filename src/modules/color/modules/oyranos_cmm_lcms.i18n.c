#define oyranos_json "{\
  \"org\": {\
    \"freedesktop\": {\
      \"oyjl\": {\
        \"comment\": \"This is the Oyjl namespace\",\
        \"modules\": [{\
            \"comment\": \"This is the Oyjl-Module-API namespace\",\
            \"comment\": \"Internationalisation: Please translate only the following keys label, name, description, help. The translated strings shall be add as new keys at the same paths level by appending .lang_country identifier. e.g. org/freedesktop/oyjl/modules/[0]/name=ExampleCMM => org/freedesktop/oyjl/modules/[0]/name.de=BeispielCMM\",\
            \"oyjl_module_api_version\": \"1\",\
            \"comment\": \"We describe here a particular CMM. Each property object contains at least one 'name' key. All values shall be strings. *nick* or *description* keys are optional. If they are not contained, fall back to *name*. Well known objects are *manufacturer*, *copyright*, *license*, *url*, *support*, *download*, *sources*, *openicc_modules_author*, *documentation* and *logo*. The *modules/[]/nick* shall contain a four byte string in as the CMM identifier.\",\
            \"type\": \"CMM\",\
            \"label\": \"CMM\",\
            \"nick\": \"lcms\",\
            \"name\": \"Little CMS\",\
            \"description\": \"Little Color Management System\",\
            \"information\": [{\
                \"type\": \"manufacturer\",\
                \"label\": \"Manufacturer\",\
                \"name\": \"Marti Maria\",\
                \"nick\": \"mm2\",\
                \"description\": \"http://www.littlecms.com\"\
              },{\
                \"type\": \"copyright\",\
                \"label\": \"Copyright\",\
                \"name\": \"Copyright (c) 1998-2008 Marti Maria Saguer\"\
              },{\
                \"type\": \"license\",\
                \"label\": \"License\",\
                \"name\": \"MIT\",\
                \"description\": \"http://www.opensource.org/licenses/mit-license.php\"\
              },{\
                \"type\": \"url\",\
                \"label\": \"URL\",\
                \"name\": \"http://www.littlecms.com\"\
              },{\
                \"type\": \"support\",\
                \"label\": \"Support\",\
                \"name\": \"http://www.littlecms.com/\"\
              },{\
                \"type\": \"download\",\
                \"label\": \"Download\",\
                \"name\": \"http://www.littlecms.com/\"\
              },{\
                \"type\": \"sources\",\
                \"label\": \"Sources\",\
                \"name\": \"http://www.littlecms.com/downloads.htm\"\
              },{\
                \"type\": \"development\",\
                \"label\": \"Development\",\
                \"name\": \"stalled\"\
              },{\
                \"type\": \"oyjl_module_author\",\
                \"label\": \"Oyjl Module Author\",\
                \"name\": \"Kai-Uwe Behrmann\",\
                \"description\": \"http://www.behrmann.name\"\
              },{\
                \"type\": \"documentation\",\
                \"label\": \"Documentation\",\
                \"name\": \"http://www.openicc.info\",\
                \"description\": \"The lcms filter is a one by one color conversion filter. It can both create a color conversion context, some precalculated for processing speed up, and the color conversion with the help of that context. The adaption part of this filter transforms the Oyranos color context, which is ICC device link based, to the internal lcms format.\"\
              }],\
            \"logo\": \"lcms_logo2\",\
            \"groups\": [{\
                \"comment\": \"Logical group\",\
                \"name\": \"Little CMS\",\
                \"description\": \"Extended Options\",\
                \"help\": \"Additional options.\",\
                \"properties\": \"h3.frame\",\
                \"groups\": [{\
                    \"comment\": \"Logical sub group for presentation. *name* might be shown as label alone or together with *description*.\",\
                    \"name\": \"CMM\",\
                    \"description\": \"Options\",\
                    \"help\": \"CMM behaviour options for color rendering and precission.\",\
                    \"properties\": \"h4\",\
                    \"options\": [{\
                        \"key\": \"org/freedesktop/openicc/icc_color/cmyk_cmyk_black_preservation\",\
                        \"default\": \"0\",\
                        \"properties\": \"rendering\",\
                        \"type\": \"choice\",\
                        \"choices\": [{\
                            \"nick\": \"0\",\
                            \"name\": \"[none]\"\
                          },{\
                            \"nick\": \"1\",\
                            \"name\": \"LCMS_PRESERVE_PURE_K\"\
                          },{\
                            \"nick\": \"2\",\
                            \"name\": \"LCMS_PRESERVE_K_PLANE\"\
                          }],\
                        \"name\": \"Black Preservation\",\
                        \"description\": \"Decide how to preserve the black channel for Cmyk to Cmyk transforms\",\
                        \"help\": \"Cmyk to Cmyk transforms can provide various strategies to preserve the black only channel. None means, black might change to Cmy and thus text prints not very well. LittleCMS has added two different modes to deal with that: Black-ink-only preservation and black-plane preservation. The first is simple and effective: do all the colorimetric transforms but keep only K (preserving L*) where the source image is only black. The second mode is fair more complex and tries to preserve the WHOLE K plane.\"\
                      },{\
                        \"key\": \"org/freedesktop/openicc/icc_color/precalculation\",\
                        \"default\": \"0\",\
                        \"properties\": \"rendering.advanced\",\
                        \"type\": \"choice\",\
                        \"choices\": [{\
                            \"nick\": \"0\",\
                            \"name\": \"normal\"\
                          },{\
                            \"nick\": \"1\",\
                            \"name\": \"LCMS_NOOPTIMIZE\"\
                          },{\
                            \"nick\": \"2\",\
                            \"name\": \"LCMS_HIGHRESPRECALC\"\
                          },{\
                            \"nick\": \"3\",\
                            \"name\": \"LCMS_LOWRESPRECALC\"\
                          }],\
                        \"name\": \"Optimization\",\
                        \"description\": \"Color Transforms can be differently stored internally\",\
                        \"help\": \"Little CMS tries to optimize profile chains whatever possible. There are some built-in optimization schemes, and you can add new schemas by using a plug-in. This generally improves the performance of the transform, but may introduce a small delay of 1-2 seconds when creating the transform. If you are going to transform just few colors, you don't need this precalculations. Then, the flag cmsFLAGS_NOOPTIMIZE in cmsCreateTransform() can be used to inhibit the optimization process. See the API reference for a more detailed discussion of the flags.\"\
                      }]\
                  }]\
              }]\
          }],\
        \"translations\": {\
          \"cs_CZ\": {\
            \"CMM\": \"CMM\",\
            \"Little CMS\": \"Little CMS\",\
            \"Manufacturer\": \"Výrobce\",\
            \"Marti Maria\": \"Marti Maria\",\
            \"Copyright\": \"Copyright\",\
            \"License\": \"Licence\",\
            \"MIT\": \"MIT\",\
            \"Kai-Uwe Behrmann\": \"Kai-Uwe Behrmann\",\
            \"Documentation\": \"Dokumentace\",\
            \"Extended Options\": \"Rozšířené volby:\",\
            \"[none]\": \"[žádný]\",\
            \"Black Preservation\": \"Zachování černé\",\
            \"Decide how to preserve the black channel for Cmyk to Cmyk transforms\": \"Rozhodnout, jakým způsobem zachovat černý kanál při transformaci CMYK -> CMYK.\",\
            \"Cmyk to Cmyk transforms can provide various strategies to preserve the black only channel. None means, black might change to Cmy and thus text prints not very well. LittleCMS has added two different modes to deal with that: Black-ink-only preservation and black-plane preservation. The first is simple and effective: do all the colorimetric transforms but keep only K (preserving L*) where the source image is only black. The second mode is fair more complex and tries to preserve the WHOLE K plane.\": \"Při transformaci CMYK -> CMYK lze použít různé strategie pro zachování černého kanálu. 'Žádný' znamená, že černá se může změnit na CMY a text se tudíž nevytiskne nejlépe. Little CMS to řeší dvěma způsoby: zachování pouze černého inkoustu nebo zachování roviny černé. První způsob je jednoduchý a účinný: provedou se všechny kolorimetrické transformace, ale zachová se K (světlost L*) jen v případech, kdy vstupní obrázek je pouze černý. Druhý způsob je značně komplexní a snaží se zachovat celou rovinu černé (K).\",\
            \"Optimization\": \"Optimalizace\",\
            \"Color Transforms can be differently stored internally\": \"Transformace barev mohou být interně uloženy různými způsoby\",\
            \"Little CMS tries to optimize profile chains whatever possible. There are some built-in optimization schemes, and you can add new schemas by using a plug-in. This generally improves the performance of the transform, but may introduce a small delay of 1-2 seconds when creating the transform. If you are going to transform just few colors, you don't need this precalculations. Then, the flag cmsFLAGS_NOOPTIMIZE in cmsCreateTransform() can be used to inhibit the optimization process. See the API reference for a more detailed discussion of the flags.\": \"Little CMS se snaží optimalizovat řetězení profilů kdykoliv je to možné. Některá optimalizační schémata jsou již přednastavena, nová schémata můžete přidat skrze plug-in. Obvykle dojde ke zlepšení výkonu transformace, ale může dojít ke zpoždění při vytváření metody transformace. Pokud budete transformovat pouze několik barev, pak nepotřebujete tyto předběžné výpočty. V takovém případě lze použít příznak cmsFLAGS_NOOPTIMIZE v cmsCreateTransform() k zabránění procesu optimalizace. Viz reference API pro detailnější diskuzi ohledně příznaků.\"\
          },\
          \"de_DE\": {\
            \"CMM\": \"CMM\",\
            \"Little CMS\": \"Little CMS\",\
            \"Little Color Management System\": \"Little Color Management System\",\
            \"Manufacturer\": \"Hersteller\",\
            \"Marti Maria\": \"Marti Maria\",\
            \"http://www.littlecms.com\": \"http://www.littlecms.com\",\
            \"Copyright\": \"Kopierrecht\",\
            \"Copyright (c) 1998-2008 Marti Maria Saguer\": \"Kopierrecht (c) 1998-2008 Marti Maria Saguer\",\
            \"License\": \"Lizenz\",\
            \"MIT\": \"MIT\",\
            \"http://www.opensource.org/licenses/mit-license.php\": \"http://www.opensource.org/licenses/mit-license.php\",\
            \"URL\": \"URL\",\
            \"http://www.littlecms.com\": \"http://www.littlecms.com\",\
            \"Support\": \"Unterstützung\",\
            \"http://www.littlecms.com/\": \"http://www.littlecms.com\",\
            \"Download\": \"Bezugsquelle\",\
            \"http://www.littlecms.com/\": \"http://www.littlecms.com\",\
            \"Sources\": \"Quellen\",\
            \"http://www.littlecms.com/downloads.htm\": \"http://www.littlecms.com/downloads.htm\",\
            \"Development\": \"Entwicklung\",\
            \"stalled\": \"nicht aktiv\",\
            \"Oyjl Module Author\": \"Oyjl Modul Autor\",\
            \"Kai-Uwe Behrmann\": \"Kai-Uwe Behrmann\",\
            \"http://www.behrmann.name\": \"http://www.behrmann.name\",\
            \"Documentation\": \"Dokumente\",\
            \"http://www.openicc.info\": \"http://www.openicc.info\",\
            \"The lcms filter is a one by one color conversion filter. It can both create a color conversion context, some precalculated for processing speed up, and the color conversion with the help of that context. The adaption part of this filter transforms the Oyranos color context, which is ICC device link based, to the internal lcms format.\": \"Das lcms Modul kann eindimensional Farben übertragen. Die CMM, Farbübertragungsmodul, berechnet die Farbübertragungsdaten vor und kann Farben mit deren Hilfe auch übertragen. Es gibt einen Anpassungsteil im Modul, welcher die interne lcms Farbübertragungsdaten an das Oyranos' Format anpasst, welches auf ICC Geräteverknüpfungen, engl. device links, beruht.\",\
            \"Extended Options\": \"Erweiterte Optionen\",\
            \"Additional options.\": \"Zusätzliche Optionen.\",\
            \"Options\": \"Optionen\",\
            \"CMM behaviour options for color rendering and precission.\": \"CMM Verhalten für Farbumwandlung und Präzission.\",\
            \"[none]\": \"[kein]\",\
            \"LCMS_PRESERVE_PURE_K\": \"LCMS_PRESERVE_PURE_K\",\
            \"LCMS_PRESERVE_K_PLANE\": \"LCMS_PRESERVE_K_PLANE\",\
            \"Black Preservation\": \"Schwarzerhalt\",\
            \"Decide how to preserve the black channel for Cmyk to Cmyk transforms\": \"Entscheide wie der Schwarzkanal erhalten wird bei Umwandlungen von einem Druckfarbraum in einen Anderen\",\
            \"normal\": \"normal\",\
            \"LCMS_NOOPTIMIZE\": \"LCMS_NOOPTIMIZE\",\
            \"LCMS_HIGHRESPRECALC\": \"LCMS_HIGHRESPRECALC\",\
            \"LCMS_LOWRESPRECALC\": \"LCMS_LOWRESPRECALC\",\
            \"Optimization\": \"Optimierung\",\
            \"Color Transforms can be differently stored internally\": \"Farbumwandlung können intern unterschiedlich gespeichert werden\"\
          },\
          \"fr_FR\": {\
            \"Copyright\": \"Copyright\"\
          }\
        }\
      }\
    }\
  }\
}"
