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
            \"nick\": \"lcm2\",\n\
            \"name\": \"Little CMS 2\",\n\
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
                \"name\": \"Copyright 2018 Marti Maria\",\n\
                \"label.cs_CZ\": \"Copyright\",\n\
                \"label.de_DE\": \"Kopierrecht\",\n\
                \"name.de_DE\": \"Kopierrecht 2018 Marti Maria\",\n\
                \"label.fr_FR\": \"Copyright\"\n\
              },{\n\
                \"type\": \"license\",\n\
                \"label\": \"License\",\n\
                \"name\": \"MIT\",\n\
                \"description\": \"http://www.littlecms.com\",\n\
                \"label.cs_CZ\": \"Licence\",\n\
                \"name.cs_CZ\": \"MIT\",\n\
                \"label.de_DE\": \"Lizenz\",\n\
                \"name.de_DE\": \"MIT\",\n\
                \"description.de_DE\": \"http://www.littlecms.com\"\n\
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
                \"name\": \"http://www.littlecms.com/\",\n\
                \"label.de_DE\": \"Quellen\",\n\
                \"name.de_DE\": \"http://www.littlecms.com\"\n\
              },{\n\
                \"type\": \"development\",\n\
                \"label\": \"Development\",\n\
                \"name\": \"https://github.com/mm2/little-cms\",\n\
                \"label.de_DE\": \"Entwicklung\"\n\
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
                \"description\": \"The module expects options for profiles and rendering_intent. Optional are rendering_bpc, cmyk_cmyk_black_preservation, precalculation_curves, adaption_state, no_white_on_white_fixup.\",\n\
                \"label.cs_CZ\": \"Dokumentace\",\n\
                \"label.de_DE\": \"Dokumente\",\n\
                \"name.de_DE\": \"http://www.openicc.info\"\n\
              }],\n\
            \"logo\": \"lcms_logo2\",\n\
            \"groups\": [{\n\
                \"comment\": \"Logical group\",\n\
                \"name\": \"Little CMS 2\",\n\
                \"description\": \"Extended Options\",\n\
                \"help\": \"Additional options.\",\n\
                \"properties\": \"h3.frame\",\n\
                \"groups\": [{\n\
                    \"comment\": \"Logical sub group for presentation. *name* might be shown as label alone or together with *description*.\",\n\
                    \"name\": \"CMM\",\n\
                    \"description\": \"Options\",\n\
                    \"help\": \"CMM behaviour options for color rendering, precission and fixes.\",\n\
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
                        \"help\": \"Cmyk to Cmyk transforms can provide various strategies to preserve the black only channel. None means, black might change to Cmy and thus text prints not very well. LittleCMS 2 has added two different modes to deal with that: Black-ink-only preservation and black-plane preservation. The first is simple and effective: do all the colorimetric transforms but keep only K (preserving L*) where the source image is only black. The second mode is fair more complex and tries to preserve the WHOLE K plane.\",\n\
                        \"name.cs_CZ\": \"Zachování černé\",\n\
                        \"description.cs_CZ\": \"Rozhodnout, jakým způsobem zachovat černý kanál při transformaci CMYK -> CMYK.\",\n\
                        \"help.cs_CZ\": \"Při transformaci CMYK -> CMYK lze použít různé strategie pro zachování černého kanálu. 'Žádný' znamená, že černá se může změnit na CMY a text se tedy nevytiskne nejlépe. LittleCMS 2 to řeší dvěma způsoby: zachování pouze černého inkoustu nebo zachování roviny černé. První způsob je jednoduchý a účinný: provedou se všechny kolorimetrické transformace, ale zachová se jen K (světlost L*) v případech, kdy vstupní obrázek je pouze černý. Druhý způsob je značně komplexní a snaží se zachovat celou rovinu černé (K).\",\n\
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
                            \"name\": \"LCMS2_NOOPTIMIZE\"\n\
                          },{\n\
                            \"nick\": \"2\",\n\
                            \"name\": \"LCMS2_HIGHRESPRECALC\"\n\
                          },{\n\
                            \"nick\": \"3\",\n\
                            \"name\": \"LCMS2_LOWRESPRECALC\"\n\
                          }],\n\
                        \"name\": \"Optimization\",\n\
                        \"description\": \"Color Transforms can be differently stored internally\",\n\
                        \"help\": \"Little CMS tries to optimize profile chains whatever possible. There are some built-in optimization schemes, and you can add new schemas by using a plug-in. This generally improves the performance of the transform, but may introduce a small delay of 1-2 seconds when creating the transform. If you are going to transform just few colors, you don't need this precalculations. Then, the flag cmsFLAGS_NOOPTIMIZE in cmsCreateTransform() can be used to inhibit the optimization process. See the API reference for a more detailed discussion of the flags.\",\n\
                        \"name.cs_CZ\": \"Optimalizace\",\n\
                        \"description.cs_CZ\": \"Transformace barev mohou být interně uloženy různými způsoby\",\n\
                        \"help.cs_CZ\": \"Little CMS se snaží optimalizovat řetězení profilů kdykoliv je to možné. Některá optimalizační schémata jsou již přednastavena, nová schémata můžete přidat skrze plug-in. Obvykle dojde ke zlepšení výkonu transformace, ale může dojít ke zpoždění při vytváření metody transformace. Pokud budete transformovat pouze několik barev, pak nepotřebujete tyto předběžné výpočty. V takovém případě lze použít příznak cmsFLAGS_NOOPTIMIZE v cmsCreateTransform() k zabránění procesu optimalizace. Viz reference API pro detailnější diskuzi ohledně příznaků.\",\n\
                        \"name.de_DE\": \"Optimierung\",\n\
                        \"description.de_DE\": \"Farbumwandlung können intern unterschiedlich gespeichert werden\"\n\
                      },{\n\
                        \"key\": \"org/freedesktop/openicc/icc_color/precalculation_curves\",\n\
                        \"default\": \"1\",\n\
                        \"properties\": \"rendering.advanced.boolean\",\n\
                        \"choices\": [{\n\
                            \"nick\": \"0\",\n\
                            \"name\": \"[none]\",\n\
                            \"name.cs_CZ\": \"[žádný]\",\n\
                            \"name.de_DE\": \"[kein]\"\n\
                          },{\n\
                            \"nick\": \"1\",\n\
                            \"name\": \"LCMS2_POST+PRE_CURVES\"\n\
                          }],\n\
                        \"name\": \"Curves for Optimization\",\n\
                        \"description\": \"Color Transform CLUT's can additionally use curves for special cases\",\n\
                        \"help\": \"Little CMS can use curves before and after CLUT's for special cases like gamma encoded values to and from linear gamma values. Performance will suffer.\",\n\
                        \"name.cs_CZ\": \"Křivky pro optimalizaci\",\n\
                        \"description.cs_CZ\": \"Transformace barev typu CLUT může ve zvláštních případech využít i křivky.\",\n\
                        \"help.cs_CZ\": \"Ve zvláštních případech jako hodnoty kódované v gama do a z lineárních hodnot může Little CMS využít křivky před a po CLUT. Výkon však poklesne.\",\n\
                        \"name.de_DE\": \"Kurven für Optimierungen\",\n\
                        \"description.de_DE\": \"Farbtransformationstabellen können für spezielle Fälle zusätzlich mit Kurven kombiniert werden\",\n\
                        \"help.de_DE\": \"Little CMS kann Kurven vor und nach 3D Interpolationstabellen benutzen. Dadurch zeigen sich weniger Fehler bei der Übertragung zwischen linearen und gammabehafteten Werten auf Kosten der Geschwindigkeit.\"\n\
                      },{\n\
                        \"key\": \"org/freedesktop/openicc/icc_color/adaption_state\",\n\
                        \"default\": \"1\",\n\
                        \"properties\": \"rendering.advanced\",\n\
                        \"choices\": [{\n\
                            \"nick\": \"0\",\n\
                            \"name\": \"0.0\",\n\
                            \"name.de_DE\": \"0,0\"\n\
                          },{\n\
                            \"nick\": \"1\",\n\
                            \"name\": \"1.0\",\n\
                            \"name.de_DE\": \"1,0\"\n\
                          }],\n\
                        \"name\": \"Adaptation State\",\n\
                        \"description\": \"Adaptation state for absolute colorimetric intent\",\n\
                        \"help\": \"The adaption state should be between 0 and 1.0 and will apply to the absolute colorimetric intent.\",\n\
                        \"name.cs_CZ\": \"Stav adaptace\",\n\
                        \"description.cs_CZ\": \"Adaptační stav pro absolutní kolorimetrický záměr\",\n\
                        \"help.cs_CZ\": \"Adaptační stav by měl být mezi 0 a 1.0 a týká se absolutního kolorimetrického záměru.\",\n\
                        \"name.de_DE\": \"Farbanpassungsgrad\",\n\
                        \"description.de_DE\": \"Farbanpassungsgrad für die absolut farbmetrische Übertragung\",\n\
                        \"help.de_DE\": \"Der Farbanpassungsgrad sollte zwischen 0 und 1.0 liegen und wird auf die absolut farbmetrische Übertragung angewendet.\"\n\
                      },{\n\
                        \"key\": \"org/freedesktop/openicc/icc_color/no_white_on_white_fixup\",\n\
                        \"default\": \"1\",\n\
                        \"properties\": \"rendering.advanced\",\n\
                        \"choices\": [{\n\
                            \"nick\": \"0\",\n\
                            \"name\": \"No\",\n\
                            \"name.cs_CZ\": \"Ne\",\n\
                            \"name.de_DE\": \"Nein\"\n\
                          },{\n\
                            \"nick\": \"1\",\n\
                            \"name\": \"Yes\",\n\
                            \"name.cs_CZ\": \"Ano\",\n\
                            \"name.de_DE\": \"Ja\",\n\
                            \"name.eo\": \"Jes\",\n\
                            \"name.eu\": \"Jes\"\n\
                          }],\n\
                        \"name\": \"No White on White Fix\",\n\
                        \"description\": \"Skip White Point on White point alignment\",\n\
                        \"help\": \"Avoid force of White on White mapping. Default for absolute rendering intent.\",\n\
                        \"name.de_DE\": \"Keine Weiß zu Weiß Reparatur\",\n\
                        \"description.de_DE\": \"Lasse Weiß auf Weiß Einpassung weg\",\n\
                        \"help.de_DE\": \"Lasse das Erzwingen einer Weiß zu Weiß übertragung fort. Standard bei Absolut Farbmetrischer Übertragung.\"\n\
                      }],\n\
                    \"name.cs_CZ\": \"CMM\",\n\
                    \"description.de_DE\": \"Optionen\",\n\
                    \"help.de_DE\": \"CMM Verhalten für Farbumwandlung, Präzission und Verbesserungen.\"\n\
                  }],\n\
                \"name.cs_CZ\": \"Little CMS 2\",\n\
                \"description.cs_CZ\": \"Rozšířené volby:\",\n\
                \"name.de_DE\": \"Little CMS 2\",\n\
                \"description.de_DE\": \"Erweiterte Optionen\",\n\
                \"help.de_DE\": \"Zusätzliche Optionen.\"\n\
              }],\n\
            \"label.cs_CZ\": \"CMM\",\n\
            \"name.cs_CZ\": \"Little CMS 2\",\n\
            \"name.de_DE\": \"Little CMS 2\",\n\
            \"description.de_DE\": \"Little Color Management System\"\n\
          }]\n\
      }\n\
    }\n\
  }\n\
}"
