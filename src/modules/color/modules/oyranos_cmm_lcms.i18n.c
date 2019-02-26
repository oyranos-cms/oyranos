#define oyranos_json "{\n\
  \"org\": {\n\
    \"freedesktop\": {\n\
      \"openicc\": {\n\
        \"comment\": \"This is the OpenICC namespace\",\n\
        \"modules\": [{\n\
            \"comment\": \"This is the OpenICC-CMM-API namespace\",\n\
            \"comment\": \"Internationalisation: Please translate only the following keys label, name, description, help. The translated strings shall be add as new keys at the same paths level by appending .lang_country identifier. e.g. org/freedesktop/openicc/modules/[0]/name=ExampleCMM => org/freedesktop/openicc/modules/[0]/name.de=BeispielCMM\",\n\
            \"openicc_module_api_version\": \"1\",\n\
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
                \"description\": \"http://www.littlecms.com\",\n\
                \"label.cs_CZ\": \"Výrobce\",\n\
                \"name.cs_CZ\": \"Marti Maria\",\n\
                \"label.de_DE\": \"Hersteller\",\n\
                \"description.de_DE\": \"http://www.littlecms.com\"\n\
              },{\n\
                \"type\": \"copyright\",\n\
                \"label\": \"Copyright\",\n\
                \"name\": \"Copyright (c) 1998-2008 Marti Maria Saguer\",\n\
                \"label.cs_CZ\": \"Copyright\",\n\
                \"label.de_DE\": \"Kopierrecht\",\n\
                \"name.de_DE\": \"Kopierrecht (c) 1998-2008 Marti Maria Saguer\",\n\
                \"label.fr_FR\": \"Copyright\"\n\
              },{\n\
                \"type\": \"license\",\n\
                \"label\": \"License\",\n\
                \"name\": \"MIT\",\n\
                \"description\": \"http://www.opensource.org/licenses/mit-license.php\",\n\
                \"label.cs_CZ\": \"Licence\",\n\
                \"name.cs_CZ\": \"MIT\",\n\
                \"label.de_DE\": \"Lizenz\",\n\
                \"name.de_DE\": \"MIT\",\n\
                \"description.de_DE\": \"http://www.opensource.org/licenses/mit-license.php\"\n\
              },{\n\
                \"type\": \"url\",\n\
                \"label\": \"URL\",\n\
                \"name\": \"http://www.littlecms.com\",\n\
                \"name.de_DE\": \"http://www.littlecms.com\"\n\
              },{\n\
                \"type\": \"support\",\n\
                \"label\": \"Support\",\n\
                \"name\": \"http://www.littlecms.com/\",\n\
                \"label.de_DE\": \"Unterstützung\",\n\
                \"name.de_DE\": \"http://www.littlecms.com\"\n\
              },{\n\
                \"type\": \"download\",\n\
                \"label\": \"Download\",\n\
                \"name\": \"http://www.littlecms.com/\",\n\
                \"label.de_DE\": \"Bezugsquelle\",\n\
                \"name.de_DE\": \"http://www.littlecms.com\"\n\
              },{\n\
                \"type\": \"sources\",\n\
                \"label\": \"Sources\",\n\
                \"name\": \"http://www.littlecms.com/downloads.htm\",\n\
                \"label.de_DE\": \"Quellen\",\n\
                \"name.de_DE\": \"http://www.littlecms.com/downloads.htm\"\n\
              },{\n\
                \"type\": \"development\",\n\
                \"label\": \"Development\",\n\
                \"name\": \"stalled\",\n\
                \"label.de_DE\": \"Entwicklung\",\n\
                \"name.de_DE\": \"nicht aktiv\"\n\
              },{\n\
                \"type\": \"openicc_module_author\",\n\
                \"label\": \"OpenICC CMM Author\",\n\
                \"name\": \"Kai-Uwe Behrmann\",\n\
                \"description\": \"http://www.behrmann.name\",\n\
                \"name.cs_CZ\": \"Kai-Uwe Behrmann\",\n\
                \"label.de_DE\": \"OpenICC CMM Autor\",\n\
                \"description.de_DE\": \"http://www.behrmann.name\"\n\
              },{\n\
                \"type\": \"documentation\",\n\
                \"label\": \"Documentation\",\n\
                \"name\": \"http://www.openicc.info\",\n\
                \"description\": \"The lcms filter is a one by one color conversion filter. It can both create a color conversion context, some precalculated for processing speed up, and the color conversion with the help of that context. The adaption part of this filter transforms the Oyranos color context, which is ICC device link based, to the internal lcms format.\",\n\
                \"label.cs_CZ\": \"Dokumentace\",\n\
                \"label.de_DE\": \"Dokumente\",\n\
                \"name.de_DE\": \"http://www.openicc.info\",\n\
                \"description.de_DE\": \"Das lcms Modul kann eindimensional Farben übertragen. Die CMM, Farbübertragungsmodul, berechnet die Farbübertragungsdaten vor und kann Farben mit deren Hilfe auch übertragen. Es gibt einen Anpassungsteil im Modul, welcher die interne lcms Farbübertragungsdaten an das Oyranos' Format anpasst, welches auf ICC Geräteverknüpfungen, engl. device links, beruht.\"\n\
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
                        \"choices\": [{\n\
                            \"nick\": \"0\",\n\
                            \"name\": \"[none]\",\n\
                            \"name.cs_CZ\": \"[žádný]\",\n\
                            \"name.de_DE\": \"[kein]\"\n\
                          },{\n\
                            \"nick\": \"1\",\n\
                            \"name\": \"LCMS_PRESERVE_PURE_K\"\n\
                          },{\n\
                            \"nick\": \"2\",\n\
                            \"name\": \"LCMS_PRESERVE_K_PLANE\"\n\
                          }],\n\
                        \"name\": \"Black Preservation\",\n\
                        \"description\": \"Decide how to preserve the black channel for Cmyk to Cmyk transforms\",\n\
                        \"help\": \"Cmyk to Cmyk transforms can provide various strategies to preserve the black only channel. None means, black might change to Cmy and thus text prints not very well. LittleCMS has added two different modes to deal with that: Black-ink-only preservation and black-plane preservation. The first is simple and effective: do all the colorimetric transforms but keep only K (preserving L*) where the source image is only black. The second mode is fair more complex and tries to preserve the WHOLE K plane.\",\n\
                        \"name.cs_CZ\": \"Zachování černé\",\n\
                        \"description.cs_CZ\": \"Rozhodnout, jakým způsobem zachovat černý kanál při transformaci CMYK -> CMYK.\",\n\
                        \"help.cs_CZ\": \"Při transformaci CMYK -> CMYK lze použít různé strategie pro zachování černého kanálu. 'Žádný' znamená, že černá se může změnit na CMY a text se tudíž nevytiskne nejlépe. Little CMS to řeší dvěma způsoby: zachování pouze černého inkoustu nebo zachování roviny černé. První způsob je jednoduchý a účinný: provedou se všechny kolorimetrické transformace, ale zachová se K (světlost L*) jen v případech, kdy vstupní obrázek je pouze černý. Druhý způsob je značně komplexní a snaží se zachovat celou rovinu černé (K).\",\n\
                        \"name.de_DE\": \"Schwarzerhalt\",\n\
                        \"description.de_DE\": \"Entscheide wie der Schwarzkanal erhalten wird bei Umwandlungen von einem Druckfarbraum in einen Anderen\"\n\
                      },{\n\
                        \"key\": \"org/freedesktop/openicc/icc_color/precalculation\",\n\
                        \"default\": \"0\",\n\
                        \"properties\": \"rendering.advanced\",\n\
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
                        \"help\": \"Little CMS tries to optimize profile chains whatever possible. There are some built-in optimization schemes, and you can add new schemas by using a plug-in. This generally improves the performance of the transform, but may introduce a small delay of 1-2 seconds when creating the transform. If you are going to transform just few colors, you don't need this precalculations. Then, the flag cmsFLAGS_NOOPTIMIZE in cmsCreateTransform() can be used to inhibit the optimization process. See the API reference for a more detailed discussion of the flags.\",\n\
                        \"name.cs_CZ\": \"Optimalizace\",\n\
                        \"description.cs_CZ\": \"Transformace barev mohou být interně uloženy různými způsoby\",\n\
                        \"help.cs_CZ\": \"Little CMS se snaží optimalizovat řetězení profilů kdykoliv je to možné. Některá optimalizační schémata jsou již přednastavena, nová schémata můžete přidat skrze plug-in. Obvykle dojde ke zlepšení výkonu transformace, ale může dojít ke zpoždění při vytváření metody transformace. Pokud budete transformovat pouze několik barev, pak nepotřebujete tyto předběžné výpočty. V takovém případě lze použít příznak cmsFLAGS_NOOPTIMIZE v cmsCreateTransform() k zabránění procesu optimalizace. Viz reference API pro detailnější diskuzi ohledně příznaků.\",\n\
                        \"name.de_DE\": \"Optimierung\",\n\
                        \"description.de_DE\": \"Farbumwandlung können intern unterschiedlich gespeichert werden\"\n\
                      }],\n\
                    \"name.cs_CZ\": \"CMM\",\n\
                    \"description.de_DE\": \"Optionen\",\n\
                    \"help.de_DE\": \"CMM Verhalten für Farbumwandlung und Präzission.\"\n\
                  }],\n\
                \"name.cs_CZ\": \"Little CMS\",\n\
                \"description.cs_CZ\": \"Rozšířené volby:\",\n\
                \"name.de_DE\": \"Little CMS\",\n\
                \"description.de_DE\": \"Erweiterte Optionen\",\n\
                \"help.de_DE\": \"Zusätzliche Optionen.\"\n\
              }],\n\
            \"label.cs_CZ\": \"CMM\",\n\
            \"name.cs_CZ\": \"Little CMS\",\n\
            \"name.de_DE\": \"Little CMS\",\n\
            \"description.de_DE\": \"Little Color Management System\"\n\
          }]\n\
      }\n\
    }\n\
  }\n\
}"
