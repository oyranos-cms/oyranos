#define oyranos_json "{\n\
  \"org\": {\n\
    \"freedesktop\": {\n\
      \"oyjl\": {\n\
        \"comment\": \"This is the Oyjl namespace\",\n\
        \"modules\": [{\n\
            \"comment\": \"This is the Oyjl-Module-API namespace\",\n\
            \"comment\": \"Internationalisation: Please translate only the following keys label, name, description, help. The translated strings shall be add as new keys at the same paths level by appending .lang_country identifier. e.g. org/freedesktop/oyjl/modules/[0]/name=ExampleCMM => org/freedesktop/oyjl/modules/[0]/name.de=BeispielCMM\",\n\
            \"oyjl_module_api_version\": \"1\",\n\
            \"comment\": \"We describe here a particular CMM. Each property object contains at least one 'name' key. All values shall be strings. *nick* or *description* keys are optional. If they are not contained, fall back to *name*. Well known objects are *manufacturer*, *copyright*, *license*, *url*, *support*, *download*, *sources*, *openicc_modules_author*, *documentation* and *logo*. The *modules/[]/nick* shall contain a four byte string in as the CMM identifier.\",\n\
            \"type\": \"CMM\",\n\
            \"label\": \"CMM\",\n\
            \"nick\": \"lcms\",\n\
            \"name\": \"Little CMS\",\n\
            \"description\": \"Little Color Management System\",\n\
            \"information\": [{\n\
                \"type\": \"manufacturer\",\n\
                \"label\": \"Manufacturer\",\n\
                \"name\": \"Marti Maria\",\n\
                \"nick\": \"mm2\",\n\
                \"description\": \"http://www.littlecms.com\"\n\
              },{\n\
                \"type\": \"copyright\",\n\
                \"label\": \"Copyright\",\n\
                \"name\": \"Copyright (c) 1998-2008 Marti Maria Saguer\"\n\
              },{\n\
                \"type\": \"license\",\n\
                \"label\": \"License\",\n\
                \"name\": \"MIT\",\n\
                \"description\": \"http://www.opensource.org/licenses/mit-license.php\"\n\
              },{\n\
                \"type\": \"url\",\n\
                \"label\": \"URL\",\n\
                \"name\": \"http://www.littlecms.com\"\n\
              },{\n\
                \"type\": \"support\",\n\
                \"label\": \"Support\",\n\
                \"name\": \"http://www.littlecms.com/\"\n\
              },{\n\
                \"type\": \"download\",\n\
                \"label\": \"Download\",\n\
                \"name\": \"http://www.littlecms.com/\"\n\
              },{\n\
                \"type\": \"sources\",\n\
                \"label\": \"Sources\",\n\
                \"name\": \"http://www.littlecms.com/downloads.htm\"\n\
              },{\n\
                \"type\": \"development\",\n\
                \"label\": \"Development\",\n\
                \"name\": \"stalled\"\n\
              },{\n\
                \"type\": \"oyjl_module_author\",\n\
                \"label\": \"Oyjl Module Author\",\n\
                \"name\": \"Kai-Uwe Behrmann\",\n\
                \"description\": \"http://www.behrmann.name\"\n\
              },{\n\
                \"type\": \"documentation\",\n\
                \"label\": \"Documentation\",\n\
                \"name\": \"http://www.openicc.info\",\n\
                \"description\": \"The lcms filter is a one by one color conversion filter. It can both create a color conversion context, some precalculated for processing speed up, and the color conversion with the help of that context. The adaption part of this filter transforms the Oyranos color context, which is ICC device link based, to the internal lcms format.\"\n\
              }],\n\
            \"logo\": \"lcms_logo2\",\n\
            \"groups\": [{\n\
                \"comment\": \"Logical group\",\n\
                \"name\": \"Little CMS\",\n\
                \"description\": \"Extended Options\",\n\
                \"help\": \"Additional options.\",\n\
                \"properties\": \"h3.frame\",\n\
                \"groups\": [{\n\
                    \"comment\": \"Logical sub group for presentation. *name* might be shown as label alone or together with *description*.\",\n\
                    \"name\": \"CMM\",\n\
                    \"description\": \"Options\",\n\
                    \"help\": \"CMM behaviour options for color rendering and precission.\",\n\
                    \"properties\": \"h4\",\n\
                    \"options\": [{\n\
                        \"key\": \"org/freedesktop/openicc/icc_color/cmyk_cmyk_black_preservation\",\n\
                        \"default\": \"0\",\n\
                        \"properties\": \"rendering\",\n\
                        \"type\": \"choice\",\n\
                        \"choices\": [{\n\
                            \"nick\": \"0\",\n\
                            \"name\": \"[none]\"\n\
                          },{\n\
                            \"nick\": \"1\",\n\
                            \"name\": \"LCMS_PRESERVE_PURE_K\"\n\
                          },{\n\
                            \"nick\": \"2\",\n\
                            \"name\": \"LCMS_PRESERVE_K_PLANE\"\n\
                          }],\n\
                        \"name\": \"Black Preservation\",\n\
                        \"description\": \"Decide how to preserve the black channel for Cmyk to Cmyk transforms\",\n\
                        \"help\": \"Cmyk to Cmyk transforms can provide various strategies to preserve the black only channel. None means, black might change to Cmy and thus text prints not very well. LittleCMS has added two different modes to deal with that: Black-ink-only preservation and black-plane preservation. The first is simple and effective: do all the colorimetric transforms but keep only K (preserving L*) where the source image is only black. The second mode is fair more complex and tries to preserve the WHOLE K plane.\"\n\
                      },{\n\
                        \"key\": \"org/freedesktop/openicc/icc_color/precalculation\",\n\
                        \"default\": \"0\",\n\
                        \"properties\": \"rendering.advanced\",\n\
                        \"type\": \"choice\",\n\
                        \"choices\": [{\n\
                            \"nick\": \"0\",\n\
                            \"name\": \"normal\"\n\
                          },{\n\
                            \"nick\": \"1\",\n\
                            \"name\": \"LCMS_NOOPTIMIZE\"\n\
                          },{\n\
                            \"nick\": \"2\",\n\
                            \"name\": \"LCMS_HIGHRESPRECALC\"\n\
                          },{\n\
                            \"nick\": \"3\",\n\
                            \"name\": \"LCMS_LOWRESPRECALC\"\n\
                          }],\n\
                        \"name\": \"Optimization\",\n\
                        \"description\": \"Color Transforms can be differently stored internally\",\n\
                        \"help\": \"Little CMS tries to optimize profile chains whatever possible. There are some built-in optimization schemes, and you can add new schemas by using a plug-in. This generally improves the performance of the transform, but may introduce a small delay of 1-2 seconds when creating the transform. If you are going to transform just few colors, you don't need this precalculations. Then, the flag cmsFLAGS_NOOPTIMIZE in cmsCreateTransform() can be used to inhibit the optimization process. See the API reference for a more detailed discussion of the flags.\"\n\
                      }]\n\
                  }]\n\
              }]\n\
          }],\n\
        \"translations\": {\n\
          \"cs_CZ\": {\n\
            \"CMM\": \"CMM\",\n\
            \"Little CMS\": \"Little CMS\",\n\
            \"Manufacturer\": \"Výrobce\",\n\
            \"Marti Maria\": \"Marti Maria\",\n\
            \"Copyright\": \"Copyright\",\n\
            \"License\": \"Licence\",\n\
            \"MIT\": \"MIT\",\n\
            \"Kai-Uwe Behrmann\": \"Kai-Uwe Behrmann\",\n\
            \"Documentation\": \"Dokumentace\",\n\
            \"Extended Options\": \"Rozšířené volby:\",\n\
            \"[none]\": \"[žádný]\",\n\
            \"Black Preservation\": \"Zachování černé\",\n\
            \"Decide how to preserve the black channel for Cmyk to Cmyk transforms\": \"Rozhodnout, jakým způsobem zachovat černý kanál při transformaci CMYK -> CMYK.\",\n\
            \"Cmyk to Cmyk transforms can provide various strategies to preserve the black only channel. None means, black might change to Cmy and thus text prints not very well. LittleCMS has added two different modes to deal with that: Black-ink-only preservation and black-plane preservation. The first is simple and effective: do all the colorimetric transforms but keep only K (preserving L*) where the source image is only black. The second mode is fair more complex and tries to preserve the WHOLE K plane.\": \"Při transformaci CMYK -> CMYK lze použít různé strategie pro zachování černého kanálu. 'Žádný' znamená, že černá se může změnit na CMY a text se tudíž nevytiskne nejlépe. Little CMS to řeší dvěma způsoby: zachování pouze černého inkoustu nebo zachování roviny černé. První způsob je jednoduchý a účinný: provedou se všechny kolorimetrické transformace, ale zachová se K (světlost L*) jen v případech, kdy vstupní obrázek je pouze černý. Druhý způsob je značně komplexní a snaží se zachovat celou rovinu černé (K).\",\n\
            \"Optimization\": \"Optimalizace\",\n\
            \"Color Transforms can be differently stored internally\": \"Transformace barev mohou být interně uloženy různými způsoby\",\n\
            \"Little CMS tries to optimize profile chains whatever possible. There are some built-in optimization schemes, and you can add new schemas by using a plug-in. This generally improves the performance of the transform, but may introduce a small delay of 1-2 seconds when creating the transform. If you are going to transform just few colors, you don't need this precalculations. Then, the flag cmsFLAGS_NOOPTIMIZE in cmsCreateTransform() can be used to inhibit the optimization process. See the API reference for a more detailed discussion of the flags.\": \"Little CMS se snaží optimalizovat řetězení profilů kdykoliv je to možné. Některá optimalizační schémata jsou již přednastavena, nová schémata můžete přidat skrze plug-in. Obvykle dojde ke zlepšení výkonu transformace, ale může dojít ke zpoždění při vytváření metody transformace. Pokud budete transformovat pouze několik barev, pak nepotřebujete tyto předběžné výpočty. V takovém případě lze použít příznak cmsFLAGS_NOOPTIMIZE v cmsCreateTransform() k zabránění procesu optimalizace. Viz reference API pro detailnější diskuzi ohledně příznaků.\"\n\
          },\n\
          \"de_DE\": {\n\
            \"Little CMS\": \"Little CMS\",\n\
            \"Little Color Management System\": \"Little Color Management System\",\n\
            \"Manufacturer\": \"Hersteller\",\n\
            \"http:%37%37www.littlecms.com\": \"http://www.littlecms.com\",\n\
            \"Copyright\": \"Kopierrecht\",\n\
            \"Copyright (c) 1998-2008 Marti Maria Saguer\": \"Kopierrecht (c) 1998-2008 Marti Maria Saguer\",\n\
            \"License\": \"Lizenz\",\n\
            \"MIT\": \"MIT\",\n\
            \"http:%37%37www.opensource.org%37licenses%37mit-license.php\": \"http://www.opensource.org/licenses/mit-license.php\",\n\
            \"Support\": \"Unterstützung\",\n\
            \"http:%37%37www.littlecms.com%37\": \"http://www.littlecms.com\",\n\
            \"Download\": \"Bezugsquelle\",\n\
            \"Sources\": \"Quellen\",\n\
            \"http:%37%37www.littlecms.com%37downloads.htm\": \"http://www.littlecms.com/downloads.htm\",\n\
            \"Development\": \"Entwicklung\",\n\
            \"stalled\": \"nicht aktiv\",\n\
            \"http:%37%37www.behrmann.name\": \"http://www.behrmann.name\",\n\
            \"Documentation\": \"Dokumente\",\n\
            \"http:%37%37www.openicc.info\": \"http://www.openicc.info\",\n\
            \"The lcms filter is a one by one color conversion filter. It can both create a color conversion context, some precalculated for processing speed up, and the color conversion with the help of that context. The adaption part of this filter transforms the Oyranos color context, which is ICC device link based, to the internal lcms format.\": \"Das lcms Modul kann eindimensional Farben übertragen. Die CMM, Farbübertragungsmodul, berechnet die Farbübertragungsdaten vor und kann Farben mit deren Hilfe auch übertragen. Es gibt einen Anpassungsteil im Modul, welcher die interne lcms Farbübertragungsdaten an das Oyranos' Format anpasst, welches auf ICC Geräteverknüpfungen, engl. device links, beruht.\",\n\
            \"Extended Options\": \"Erweiterte Optionen\",\n\
            \"Additional options.\": \"Zusätzliche Optionen.\",\n\
            \"Options\": \"Optionen\",\n\
            \"CMM behaviour options for color rendering and precission.\": \"CMM Verhalten für Farbumwandlung und Präzission.\",\n\
            \"[none]\": \"[kein]\",\n\
            \"Black Preservation\": \"Schwarzerhalt\",\n\
            \"Decide how to preserve the black channel for Cmyk to Cmyk transforms\": \"Entscheide wie der Schwarzkanal erhalten wird bei Umwandlungen von einem Druckfarbraum in einen Anderen\",\n\
            \"normal\": \"normal\",\n\
            \"Optimization\": \"Optimierung\",\n\
            \"Color Transforms can be differently stored internally\": \"Farbumwandlung können intern unterschiedlich gespeichert werden\"\n\
          },\n\
          \"fr_FR\": {\n\
            \"Copyright\": \"Copyright\"\n\
          }\n\
        }\n\
      }\n\
    }\n\
  }\n\
}"
