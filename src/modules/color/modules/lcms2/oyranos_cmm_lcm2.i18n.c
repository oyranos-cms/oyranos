#define oyranos_json "{\n\
  \"org\": {\n\
    \"freedesktop\": {\n\
      \"oyjl\": {\n\
        \"comment\": \"This is the Oyjl namespace\",\n\
        \"modules\": [{\n\
            \"comment\": \"This is the Oyjl-Module-API namespace\",\n\
            \"comment\": \"Internationalisation: Please translate only the following keys label, name, description, help. The translated strings shall be add as new keys at the same paths level by appending .lang_country identifier. e.g. org/freedesktop/oyjl/modules/[0]/name=ExampleCMM => org/freedesktop/oyjl/modules/[0]/name.de=BeispielCMM\",\n\
            \"oyjl_module_api_version\": \"1\",\n\
            \"comment\": \"We describe here a particular CMM. Each property object contains at least one 'name' key. All values shall be strings. *nick* or *description* keys are optional. If they are not contained, fall back to *name*. Well known objects are *manufacturer*, *copyright*, *license*, *url*, *support*, *download*, *sources*, *oyjl_modules_author*, *documentation* and *logo*. The *modules/[]/nick* shall contain a four byte string in as the CMM identifier.\",\n\
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
                \"description\": \"http://www.littlecms.com\"\n\
              },{\n\
                \"type\": \"copyright\",\n\
                \"label\": \"Copyright\",\n\
                \"name\": \"Copyright 2018 Marti Maria\"\n\
              },{\n\
                \"type\": \"license\",\n\
                \"label\": \"License\",\n\
                \"name\": \"MIT\",\n\
                \"description\": \"http://www.littlecms.com\"\n\
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
                \"name\": \"http://www.littlecms.com/\"\n\
              },{\n\
                \"type\": \"development\",\n\
                \"label\": \"Development\",\n\
                \"name\": \"https://github.com/mm2/little-cms\"\n\
              },{\n\
                \"type\": \"oyjl_module_author\",\n\
                \"label\": \"Oyjl Module Author\",\n\
                \"name\": \"Kai-Uwe Behrmann\",\n\
                \"description\": \"http://www.behrmann.name\"\n\
              },{\n\
                \"type\": \"documentation\",\n\
                \"label\": \"Documentation\",\n\
                \"name\": \"http://www.openicc.info\",\n\
                \"description\": \"The module expects options for profiles and rendering_intent. Optional are rendering_bpc, cmyk_cmyk_black_preservation, precalculation_curves, adaption_state, no_white_on_white_fixup.\"\n\
              }],\n\
            \"logo\": \"lcms_logo2\",\n\
            \"groups\": [{\n\
                \"comment\": \"Logical group\",\n\
                \"name\": \"Little CMS 2\",\n\
                \"properties\": \"h3.frame\",\n\
                \"groups\": [{\n\
                    \"comment\": \"Logical sub group for presentation. *name* might be shown as label alone or together with *description*.\",\n\
                    \"name\": \"lcm2 Specific Options\",\n\
                    \"description\": \"Set lcm2 Options\",\n\
                    \"help\": \"CMM options for color rendering, precission and fixes.\",\n\
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
                        \"help\": \"Cmyk to Cmyk transforms can provide various strategies to preserve the black only channel. None means, black might change to Cmy and thus text prints not very well. LittleCMS 2 has added two different modes to deal with that: Black-ink-only preservation and black-plane preservation. The first is simple and effective: do all the colorimetric transforms but keep only K (preserving L*) where the source image is only black. The second mode is fair more complex and tries to preserve the WHOLE K plane.\"\n\
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
                            \"name\": \"LCMS2_NOOPTIMIZE\"\n\
                          },{\n\
                            \"nick\": \"2\",\n\
                            \"name\": \"LCMS2_HIGHRESPRECALC\"\n\
                          },{\n\
                            \"nick\": \"3\",\n\
                            \"name\": \"LCMS2_LOWRESPRECALC\"\n\
                          },{\n\
                            \"nick\": \"4\",\n\
                            \"name\": \"LCMS2_NOTRANSFORM\"\n\
                          }],\n\
                        \"name\": \"Optimization\",\n\
                        \"description\": \"Color Transforms can be differently stored internally\",\n\
                        \"help\": \"Little CMS tries to optimize profile chains whatever possible. There are some built-in optimization schemes, and you can add new schemas by using a plug-in. This generally improves the performance of the transform, but may introduce a small delay of 1-2 seconds when creating the transform. If you are going to transform just few colors, you don't need this precalculations. Then, the flag cmsFLAGS_NOOPTIMIZE in cmsCreateTransform() can be used to inhibit the optimization process. See the API reference for a more detailed discussion of the flags.\"\n\
                      },{\n\
                        \"key\": \"org/freedesktop/openicc/icc_color/precalculation_curves\",\n\
                        \"default\": \"1\",\n\
                        \"properties\": \"rendering.advanced.boolean\",\n\
                        \"type\": \"choice\",\n\
                        \"choices\": [{\n\
                            \"nick\": \"0\",\n\
                            \"name\": \"[none]\"\n\
                          },{\n\
                            \"nick\": \"1\",\n\
                            \"name\": \"LCMS2_POST+PRE_CURVES\"\n\
                          }],\n\
                        \"name\": \"Curves for Optimization\",\n\
                        \"description\": \"Color Transform CLUT's can additionally use curves for special cases\",\n\
                        \"help\": \"Little CMS can use curves before and after CLUT's for special cases like gamma encoded values to and from linear gamma values. Performance will suffer.\"\n\
                      },{\n\
                        \"key\": \"org/freedesktop/openicc/icc_color/adaption_state\",\n\
                        \"default\": \"1\",\n\
                        \"properties\": \"rendering.advanced\",\n\
                        \"type\": \"choice\",\n\
                        \"choices\": [{\n\
                            \"nick\": \"0\",\n\
                            \"name\": \"0.0\"\n\
                          },{\n\
                            \"nick\": \"1\",\n\
                            \"name\": \"1.0\"\n\
                          }],\n\
                        \"name\": \"Adaptation State\",\n\
                        \"description\": \"Adaptation state for absolute colorimetric intent\",\n\
                        \"help\": \"The adaption state should be between 0 and 1.0 and will apply to the absolute colorimetric intent.\"\n\
                      },{\n\
                        \"key\": \"org/freedesktop/openicc/icc_color/no_white_on_white_fixup\",\n\
                        \"default\": \"1\",\n\
                        \"properties\": \"rendering.advanced\",\n\
                        \"type\": \"bool\",\n\
                        \"choices\": [{\n\
                            \"nick\": \"0\",\n\
                            \"name\": \"No\"\n\
                          },{\n\
                            \"nick\": \"1\",\n\
                            \"name\": \"Yes\"\n\
                          }],\n\
                        \"name\": \"No White on White Fix\",\n\
                        \"description\": \"Skip White Point on White point alignment\",\n\
                        \"help\": \"Avoid force of White on White mapping. Default for absolute rendering intent.\"\n\
                      }]\n\
                  }]\n\
              }]\n\
          }],\n\
        \"translations\": {\n\
          \"cs_CZ\": {\n\
            \"CMM\": \"CMM\",\n\
            \"Little CMS 2\": \"Little CMS 2\",\n\
            \"Manufacturer\": \"Výrobce\",\n\
            \"Marti Maria\": \"Marti Maria\",\n\
            \"Copyright\": \"Copyright\",\n\
            \"License\": \"Licence\",\n\
            \"MIT\": \"MIT\",\n\
            \"Kai-Uwe Behrmann\": \"Kai-Uwe Behrmann\",\n\
            \"Documentation\": \"Dokumentace\",\n\
            \"[none]\": \"[žádný]\",\n\
            \"Black Preservation\": \"Zachování černé\",\n\
            \"Decide how to preserve the black channel for Cmyk to Cmyk transforms\": \"Rozhodnout, jakým způsobem zachovat černý kanál při transformaci CMYK -> CMYK.\",\n\
            \"Cmyk to Cmyk transforms can provide various strategies to preserve the black only channel. None means, black might change to Cmy and thus text prints not very well. LittleCMS 2 has added two different modes to deal with that: Black-ink-only preservation and black-plane preservation. The first is simple and effective: do all the colorimetric transforms but keep only K (preserving L*) where the source image is only black. The second mode is fair more complex and tries to preserve the WHOLE K plane.\": \"Při transformaci CMYK -> CMYK lze použít různé strategie pro zachování černého kanálu. 'Žádný' znamená, že černá se může změnit na CMY a text se tedy nevytiskne nejlépe. LittleCMS 2 to řeší dvěma způsoby: zachování pouze černého inkoustu nebo zachování roviny černé. První způsob je jednoduchý a účinný: provedou se všechny kolorimetrické transformace, ale zachová se jen K (světlost L*) v případech, kdy vstupní obrázek je pouze černý. Druhý způsob je značně komplexní a snaží se zachovat celou rovinu černé (K).\",\n\
            \"Optimization\": \"Optimalizace\",\n\
            \"Color Transforms can be differently stored internally\": \"Transformace barev mohou být interně uloženy různými způsoby\",\n\
            \"Little CMS tries to optimize profile chains whatever possible. There are some built-in optimization schemes, and you can add new schemas by using a plug-in. This generally improves the performance of the transform, but may introduce a small delay of 1-2 seconds when creating the transform. If you are going to transform just few colors, you don't need this precalculations. Then, the flag cmsFLAGS_NOOPTIMIZE in cmsCreateTransform() can be used to inhibit the optimization process. See the API reference for a more detailed discussion of the flags.\": \"Little CMS se snaží optimalizovat řetězení profilů kdykoliv je to možné. Některá optimalizační schémata jsou již přednastavena, nová schémata můžete přidat skrze plug-in. Obvykle dojde ke zlepšení výkonu transformace, ale může dojít ke zpoždění při vytváření metody transformace. Pokud budete transformovat pouze několik barev, pak nepotřebujete tyto předběžné výpočty. V takovém případě lze použít příznak cmsFLAGS_NOOPTIMIZE v cmsCreateTransform() k zabránění procesu optimalizace. Viz reference API pro detailnější diskuzi ohledně příznaků.\",\n\
            \"Curves for Optimization\": \"Křivky pro optimalizaci\",\n\
            \"Color Transform CLUT's can additionally use curves for special cases\": \"Transformace barev typu CLUT může ve zvláštních případech využít i křivky.\",\n\
            \"Little CMS can use curves before and after CLUT's for special cases like gamma encoded values to and from linear gamma values. Performance will suffer.\": \"Ve zvláštních případech jako hodnoty kódované v gama do a z lineárních hodnot může Little CMS využít křivky před a po CLUT. Výkon však poklesne.\",\n\
            \"Adaptation State\": \"Stav adaptace\",\n\
            \"Adaptation state for absolute colorimetric intent\": \"Adaptační stav pro absolutní kolorimetrický záměr\",\n\
            \"The adaption state should be between 0 and 1.0 and will apply to the absolute colorimetric intent.\": \"Adaptační stav by měl být mezi 0 a 1.0 a týká se absolutního kolorimetrického záměru.\",\n\
            \"No\": \"Ne\",\n\
            \"Yes\": \"Ano\"\n\
          },\n\
          \"de_DE\": {\n\
            \"Little CMS 2\": \"Little CMS 2\",\n\
            \"Little Color Management System\": \"Little Color Management System\",\n\
            \"Manufacturer\": \"Hersteller\",\n\
            \"http:%37%37www.littlecms.com\": \"http://www.littlecms.com\",\n\
            \"Copyright\": \"Kopierrecht\",\n\
            \"Copyright 2018 Marti Maria\": \"Kopierrecht 2018 Marti Maria\",\n\
            \"License\": \"Lizenz\",\n\
            \"MIT\": \"MIT\",\n\
            \"Support\": \"Unterstützung\",\n\
            \"http:%37%37www.littlecms.com%37\": \"http://www.littlecms.com\",\n\
            \"Download\": \"Bezugsquelle\",\n\
            \"Sources\": \"Quellen\",\n\
            \"Development\": \"Entwicklung\",\n\
            \"Oyjl Module Author\": \"Oyjl Modul Autor\",\n\
            \"http:%37%37www.behrmann.name\": \"http://www.behrmann.name\",\n\
            \"Documentation\": \"Dokumente\",\n\
            \"http:%37%37www.openicc.info\": \"http://www.openicc.info\",\n\
            \"The module expects options for profiles and rendering_intent. Optional are rendering_bpc, cmyk_cmyk_black_preservation, precalculation_curves, adaption_state, no_white_on_white_fixup.\": \"Das Modul erwartet Optionen für Profile und Übertragungsfunktion. Optional sind rendering_bpc, cmyk_cmyk_black_preservation, precalculation_curves, adaption_state, no_white_on_white_fixup.\",\n\
            \"lcm2 Specific Options\": \"lcm2 Spezifische Optionen\",\n\
            \"Set lcm2 Options\": \"Editiere lcm2 Optionen\",\n\
            \"CMM options for color rendering, precission and fixes.\": \"CMM Optionen für Farbumwandlung, Präzission und Verbesserungen.\",\n\
            \"[none]\": \"[kein]\",\n\
            \"Black Preservation\": \"Schwarzerhalt\",\n\
            \"Decide how to preserve the black channel for Cmyk to Cmyk transforms\": \"Entscheide wie der Schwarzkanal erhalten wird bei Umwandlungen von einem Druckfarbraum in einen Anderen\",\n\
            \"normal\": \"normal\",\n\
            \"Optimization\": \"Optimierung\",\n\
            \"Color Transforms can be differently stored internally\": \"Farbumwandlung können intern unterschiedlich gespeichert werden\",\n\
            \"Curves for Optimization\": \"Kurven für Optimierungen\",\n\
            \"Color Transform CLUT's can additionally use curves for special cases\": \"Farbtransformationstabellen können für spezielle Fälle zusätzlich mit Kurven kombiniert werden\",\n\
            \"Little CMS can use curves before and after CLUT's for special cases like gamma encoded values to and from linear gamma values. Performance will suffer.\": \"Little CMS kann Kurven vor und nach 3D Interpolationstabellen benutzen. Dadurch zeigen sich weniger Fehler bei der Übertragung zwischen linearen und gammabehafteten Werten auf Kosten der Geschwindigkeit.\",\n\
            \"0.0\": \"0,0\",\n\
            \"1.0\": \"1,0\",\n\
            \"Adaptation State\": \"Farbanpassungsgrad\",\n\
            \"Adaptation state for absolute colorimetric intent\": \"Farbanpassungsgrad für die absolut farbmetrische Übertragung\",\n\
            \"The adaption state should be between 0 and 1.0 and will apply to the absolute colorimetric intent.\": \"Der Farbanpassungsgrad sollte zwischen 0 und 1.0 liegen und wird auf die absolut farbmetrische Übertragung angewendet.\",\n\
            \"No\": \"Nein\",\n\
            \"Yes\": \"Ja\",\n\
            \"No White on White Fix\": \"Keine Weiß zu Weiß Reparatur\",\n\
            \"Skip White Point on White point alignment\": \"Lasse Weiß auf Weiß Einpassung weg\",\n\
            \"Avoid force of White on White mapping. Default for absolute rendering intent.\": \"Lasse das Erzwingen einer Weiß zu Weiß übertragung fort. Standard bei Absolut Farbmetrischer Übertragung.\"\n\
          },\n\
          \"eo\": {\n\
            \"Yes\": \"Jes\"\n\
          },\n\
          \"fr_FR\": {\n\
            \"Copyright\": \"Copyright\"\n\
          }\n\
        }\n\
      }\n\
    }\n\
  }\n\
}"
