/*
 * Copyright (C) 2023  Alister Sanders
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <stdlib.h>

#include "test_common.h"
#include <CUnit/Basic.h>
#include <magpie/collections/hashmap.h>
#include <magpie/collections/list.h>
#include <magpie/compare.h>
#include <magpie/hash.h>

struct entry {
    char* key;
    char* value;
};

struct entry_visited {
    char* key;
    int visited;
};

static struct entry large_entries[] = {
    {.key = "tender",         .value = "tender"       },
    { .key = "various",       .value = "various"      },
    { .key = "grab",          .value = "grab"         },
    { .key = "tested",        .value = "tested"       },
    { .key = "calculate",     .value = "calculate"    },
    { .key = "lyrical",       .value = "lyrical"      },
    { .key = "record",        .value = "record"       },
    { .key = "serious",       .value = "serious"      },
    { .key = "society",       .value = "society"      },
    { .key = "wooden",        .value = "wooden"       },
    { .key = "hushed",        .value = "hushed"       },
    { .key = "known",         .value = "known"        },
    { .key = "boring",        .value = "boring"       },
    { .key = "bucket",        .value = "bucket"       },
    { .key = "zipper",        .value = "zipper"       },
    { .key = "neat",          .value = "neat"         },
    { .key = "prevent",       .value = "prevent"      },
    { .key = "raspy",         .value = "raspy"        },
    { .key = "save",          .value = "save"         },
    { .key = "drown",         .value = "drown"        },
    { .key = "slap",          .value = "slap"         },
    { .key = "forgetful",     .value = "forgetful"    },
    { .key = "recess",        .value = "recess"       },
    { .key = "itchy",         .value = "itchy"        },
    { .key = "curly",         .value = "curly"        },
    { .key = "public",        .value = "public"       },
    { .key = "protest",       .value = "protest"      },
    { .key = "ear",           .value = "ear"          },
    { .key = "hysterical",    .value = "hysterical"   },
    { .key = "bottle",        .value = "bottle"       },
    { .key = "enjoy",         .value = "enjoy"        },
    { .key = "amuck",         .value = "amuck"        },
    { .key = "invincible",    .value = "invincible"   },
    { .key = "sugar",         .value = "sugar"        },
    { .key = "dislike",       .value = "dislike"      },
    { .key = "coal",          .value = "coal"         },
    { .key = "overt",         .value = "overt"        },
    { .key = "kitty",         .value = "kitty"        },
    { .key = "year",          .value = "year"         },
    { .key = "mighty",        .value = "mighty"       },
    { .key = "desire",        .value = "desire"       },
    { .key = "subtract",      .value = "subtract"     },
    { .key = "colour",        .value = "colour"       },
    { .key = "complete",      .value = "complete"     },
    { .key = "cheese",        .value = "cheese"       },
    { .key = "plane",         .value = "plane"        },
    { .key = "mysterious",    .value = "mysterious"   },
    { .key = "word",          .value = "word"         },
    { .key = "faulty",        .value = "faulty"       },
    { .key = "chin",          .value = "chin"         },
    { .key = "frail",         .value = "frail"        },
    { .key = "rhetorical",    .value = "rhetorical"   },
    { .key = "bright",        .value = "bright"       },
    { .key = "fire",          .value = "fire"         },
    { .key = "stay",          .value = "stay"         },
    { .key = "acid",          .value = "acid"         },
    { .key = "cactus",        .value = "cactus"       },
    { .key = "stroke",        .value = "stroke"       },
    { .key = "hungry",        .value = "hungry"       },
    { .key = "cheat",         .value = "cheat"        },
    { .key = "airport",       .value = "airport"      },
    { .key = "drop",          .value = "drop"         },
    { .key = "kindhearted",   .value = "kindhearted"  },
    { .key = "scrape",        .value = "scrape"       },
    { .key = "psychedelic",   .value = "psychedelic"  },
    { .key = "grain",         .value = "grain"        },
    { .key = "umbrella",      .value = "umbrella"     },
    { .key = "north",         .value = "north"        },
    { .key = "broad",         .value = "broad"        },
    { .key = "cap",           .value = "cap"          },
    { .key = "hapless",       .value = "hapless"      },
    { .key = "trite",         .value = "trite"        },
    { .key = "cannon",        .value = "cannon"       },
    { .key = "curious",       .value = "curious"      },
    { .key = "oceanic",       .value = "oceanic"      },
    { .key = "respect",       .value = "respect"      },
    { .key = "dare",          .value = "dare"         },
    { .key = "education",     .value = "education"    },
    { .key = "rightful",      .value = "rightful"     },
    { .key = "vague",         .value = "vague"        },
    { .key = "naughty",       .value = "naughty"      },
    { .key = "downtown",      .value = "downtown"     },
    { .key = "noiseless",     .value = "noiseless"    },
    { .key = "wrist",         .value = "wrist"        },
    { .key = "show",          .value = "show"         },
    { .key = "thinkable",     .value = "thinkable"    },
    { .key = "glamorous",     .value = "glamorous"    },
    { .key = "plan",          .value = "plan"         },
    { .key = "dependent",     .value = "dependent"    },
    { .key = "toothpaste",    .value = "toothpaste"   },
    { .key = "cheerful",      .value = "cheerful"     },
    { .key = "heartbreaking", .value = "heartbreaking"},
    { .key = "electric",      .value = "electric"     },
    { .key = "baseball",      .value = "baseball"     },
    { .key = "support",       .value = "support"      },
    { .key = "nest",          .value = "nest"         },
    { .key = "idiotic",       .value = "idiotic"      },
    { .key = "truculent",     .value = "truculent"    },
    { .key = "clip",          .value = "clip"         },
    { .key = "glorious",      .value = "glorious"     },
    { .key = "page",          .value = "page"         },
    { .key = "tricky",        .value = "tricky"       },
    { .key = "invent",        .value = "invent"       },
    { .key = "tongue",        .value = "tongue"       },
    { .key = "abounding",     .value = "abounding"    },
    { .key = "carve",         .value = "carve"        },
    { .key = "ragged",        .value = "ragged"       },
    { .key = "gaze",          .value = "gaze"         },
    { .key = "accept",        .value = "accept"       },
    { .key = "long",          .value = "long"         },
    { .key = "pushy",         .value = "pushy"        },
    { .key = "sidewalk",      .value = "sidewalk"     },
    { .key = "money",         .value = "money"        },
    { .key = "phobic",        .value = "phobic"       },
    { .key = "violent",       .value = "violent"      },
    { .key = "dysfunctional", .value = "dysfunctional"},
    { .key = "colorful",      .value = "colorful"     },
    { .key = "rainstorm",     .value = "rainstorm"    },
    { .key = "class",         .value = "class"        },
    { .key = "bag",           .value = "bag"          },
    { .key = "scold",         .value = "scold"        },
    { .key = "wholesale",     .value = "wholesale"    },
    { .key = "plain",         .value = "plain"        },
    { .key = "filthy",        .value = "filthy"       },
    { .key = "wide",          .value = "wide"         },
    { .key = "instinctive",   .value = "instinctive"  },
    { .key = "chivalrous",    .value = "chivalrous"   },
    { .key = "childlike",     .value = "childlike"    },
    { .key = "lopsided",      .value = "lopsided"     },
    { .key = "incompetent",   .value = "incompetent"  },
    { .key = "behavior",      .value = "behavior"     },
    { .key = "longing",       .value = "longing"      },
    { .key = "wonderful",     .value = "wonderful"    },
    { .key = "playground",    .value = "playground"   },
    { .key = "accessible",    .value = "accessible"   },
    { .key = "scientific",    .value = "scientific"   },
    { .key = "grease",        .value = "grease"       },
    { .key = "five",          .value = "five"         },
    { .key = "judicious",     .value = "judicious"    },
    { .key = "scissors",      .value = "scissors"     },
    { .key = "film",          .value = "film"         },
    { .key = "available",     .value = "available"    },
    { .key = "sticky",        .value = "sticky"       },
    { .key = "error",         .value = "error"        },
    { .key = "false",         .value = "false"        },
    { .key = "embarrass",     .value = "embarrass"    },
    { .key = "yawn",          .value = "yawn"         },
    { .key = "inject",        .value = "inject"       },
    { .key = "oven",          .value = "oven"         },
    { .key = "happen",        .value = "happen"       },
    { .key = "groovy",        .value = "groovy"       },
    { .key = "driving",       .value = "driving"      },
    { .key = "tour",          .value = "tour"         },
    { .key = "scratch",       .value = "scratch"      },
    { .key = "servant",       .value = "servant"      },
    { .key = "poised",        .value = "poised"       },
    { .key = "animated",      .value = "animated"     },
    { .key = "shaky",         .value = "shaky"        },
    { .key = "pine",          .value = "pine"         },
    { .key = "hurried",       .value = "hurried"      },
    { .key = "materialistic", .value = "materialistic"},
    { .key = "young",         .value = "young"        },
    { .key = "secretive",     .value = "secretive"    },
    { .key = "own",           .value = "own"          },
    { .key = "toys",          .value = "toys"         },
    { .key = "comb",          .value = "comb"         },
    { .key = "tense",         .value = "tense"        },
    { .key = "letter",        .value = "letter"       },
    { .key = "quince",        .value = "quince"       },
    { .key = "ill",           .value = "ill"          },
    { .key = "mate",          .value = "mate"         },
    { .key = "control",       .value = "control"      },
    { .key = "equal",         .value = "equal"        },
    { .key = "fast",          .value = "fast"         },
    { .key = "grip",          .value = "grip"         },
    { .key = "design",        .value = "design"       },
    { .key = "purring",       .value = "purring"      },
    { .key = "royal",         .value = "royal"        },
    { .key = "sniff",         .value = "sniff"        },
    { .key = "phone",         .value = "phone"        },
    { .key = "aspiring",      .value = "aspiring"     },
    { .key = "used",          .value = "used"         },
    { .key = "creature",      .value = "creature"     },
    { .key = "brown",         .value = "brown"        },
    { .key = "legal",         .value = "legal"        },
    { .key = "paint",         .value = "paint"        },
    { .key = "gullible",      .value = "gullible"     },
    { .key = "three",         .value = "three"        },
    { .key = "cars",          .value = "cars"         },
    { .key = "stitch",        .value = "stitch"       },
    { .key = "ship",          .value = "ship"         },
    { .key = "swift",         .value = "swift"        },
    { .key = "tasteful",      .value = "tasteful"     },
    { .key = "porter",        .value = "porter"       },
    { .key = "popcorn",       .value = "popcorn"      },
    { .key = "river",         .value = "river"        },
    { .key = "snow",          .value = "snow"         },
    { .key = "verdant",       .value = "verdant"      },
    { .key = "position",      .value = "position"     },
    { .key = "terrific",      .value = "terrific"     },
    { .key = "soak",          .value = "soak"         },
    { .key = "whip",          .value = "whip"         },
    { .key = "deep",          .value = "deep"         },
    { .key = "bouncy",        .value = "bouncy"       },
    { .key = "owe",           .value = "owe"          },
    { .key = "telling",       .value = "telling"      },
    { .key = "wobble",        .value = "wobble"       },
    { .key = "mere",          .value = "mere"         },
    { .key = "clever",        .value = "clever"       },
    { .key = "competition",   .value = "competition"  },
    { .key = "poison",        .value = "poison"       },
    { .key = "hideous",       .value = "hideous"      },
    { .key = "arm",           .value = "arm"          },
    { .key = "apparatus",     .value = "apparatus"    },
    { .key = "fang",          .value = "fang"         },
    { .key = "base",          .value = "base"         },
    { .key = "decorate",      .value = "decorate"     },
    { .key = "doctor",        .value = "doctor"       },
    { .key = "average",       .value = "average"      },
    { .key = "parcel",        .value = "parcel"       },
    { .key = "skate",         .value = "skate"        },
    { .key = "condition",     .value = "condition"    },
    { .key = "oranges",       .value = "oranges"      },
    { .key = "astonishing",   .value = "astonishing"  },
    { .key = "creator",       .value = "creator"      },
    { .key = "rifle",         .value = "rifle"        },
    { .key = "auspicious",    .value = "auspicious"   },
    { .key = "dry",           .value = "dry"          },
    { .key = "visit",         .value = "visit"        },
    { .key = "limping",       .value = "limping"      },
    { .key = "wax",           .value = "wax"          },
    { .key = "butter",        .value = "butter"       },
    { .key = "dapper",        .value = "dapper"       },
    { .key = "liquid",        .value = "liquid"       },
    { .key = "cold",          .value = "cold"         },
    { .key = "cook",          .value = "cook"         },
    { .key = "hissing",       .value = "hissing"      },
    { .key = "peel",          .value = "peel"         },
    { .key = "activity",      .value = "activity"     },
    { .key = "mountainous",   .value = "mountainous"  },
    { .key = "ring",          .value = "ring"         },
    { .key = "calculator",    .value = "calculator"   },
    { .key = "fowl",          .value = "fowl"         },
    { .key = "aggressive",    .value = "aggressive"   },
    { .key = "permissible",   .value = "permissible"  },
    { .key = "unusual",       .value = "unusual"      },
    { .key = "yard",          .value = "yard"         },
    { .key = "embarrassed",   .value = "embarrassed"  },
    { .key = "tumble",        .value = "tumble"       },
    { .key = "drag",          .value = "drag"         },
    { .key = "bounce",        .value = "bounce"       },
    { .key = "ugly",          .value = "ugly"         },
    { .key = "waste",         .value = "waste"        },
    { .key = "scintillating", .value = "scintillating"},
    { .key = "shut",          .value = "shut"         },
    { .key = "rob",           .value = "rob"          },
    { .key = "rail",          .value = "rail"         },
    { .key = "tow",           .value = "tow"          },
    { .key = "juice",         .value = "juice"        },
    { .key = "cuddly",        .value = "cuddly"       },
    { .key = "unhealthy",     .value = "unhealthy"    },
    { .key = "rock",          .value = "rock"         },
    { .key = "capricious",    .value = "capricious"   },
    { .key = "damaged",       .value = "damaged"      },
    { .key = "shoes",         .value = "shoes"        },
    { .key = "hunt",          .value = "hunt"         },
    { .key = "soggy",         .value = "soggy"        },
    { .key = "crush",         .value = "crush"        },
    { .key = "stove",         .value = "stove"        },
    { .key = "hanging",       .value = "hanging"      },
    { .key = "cracker",       .value = "cracker"      },
    { .key = "unpack",        .value = "unpack"       },
    { .key = "surprise",      .value = "surprise"     },
    { .key = "marry",         .value = "marry"        },
    { .key = "veil",          .value = "veil"         },
    { .key = "ethereal",      .value = "ethereal"     },
    { .key = "sort",          .value = "sort"         },
    { .key = "learn",         .value = "learn"        },
    { .key = "circle",        .value = "circle"       },
    { .key = "bat",           .value = "bat"          },
    { .key = "pickle",        .value = "pickle"       },
    { .key = "measure",       .value = "measure"      },
    { .key = "frighten",      .value = "frighten"     },
    { .key = "press",         .value = "press"        },
    { .key = "dam",           .value = "dam"          },
    { .key = "muscle",        .value = "muscle"       },
    { .key = "tank",          .value = "tank"         },
    { .key = "lame",          .value = "lame"         },
    { .key = "reduce",        .value = "reduce"       },
    { .key = "amazing",       .value = "amazing"      },
    { .key = "skin",          .value = "skin"         },
    { .key = "solid",         .value = "solid"        },
    { .key = "boast",         .value = "boast"        },
    { .key = "debt",          .value = "debt"         },
    { .key = "march",         .value = "march"        },
    { .key = "tremendous",    .value = "tremendous"   },
    { .key = "limit",         .value = "limit"        },
    { .key = "keen",          .value = "keen"         },
    { .key = "rings",         .value = "rings"        },
    { .key = "manage",        .value = "manage"       },
    { .key = "ticket",        .value = "ticket"       },
    { .key = "fax",           .value = "fax"          },
    { .key = "dock",          .value = "dock"         },
    { .key = "cheer",         .value = "cheer"        },
    { .key = "crib",          .value = "crib"         },
    { .key = "root",          .value = "root"         },
    { .key = "hellish",       .value = "hellish"      },
    { .key = "story",         .value = "story"        },
    { .key = "launch",        .value = "launch"       },
    { .key = "snakes",        .value = "snakes"       },
    { .key = "nippy",         .value = "nippy"        },
    { .key = "roomy",         .value = "roomy"        },
    { .key = "rinse",         .value = "rinse"        },
    { .key = "reward",        .value = "reward"       },
    { .key = "obnoxious",     .value = "obnoxious"    },
    { .key = "jaded",         .value = "jaded"        },
    { .key = "identify",      .value = "identify"     },
    { .key = "plough",        .value = "plough"       },
    { .key = "tan",           .value = "tan"          },
    { .key = "carry",         .value = "carry"        },
    { .key = "snatch",        .value = "snatch"       },
    { .key = "wall",          .value = "wall"         },
    { .key = "unwieldy",      .value = "unwieldy"     },
    { .key = "honorable",     .value = "honorable"    },
    { .key = "squealing",     .value = "squealing"    },
    { .key = "sin",           .value = "sin"          },
    { .key = "standing",      .value = "standing"     },
    { .key = "weigh",         .value = "weigh"        },
    { .key = "help",          .value = "help"         },
    { .key = "reaction",      .value = "reaction"     },
    { .key = "legs",          .value = "legs"         },
    { .key = "contain",       .value = "contain"      },
    { .key = "simple",        .value = "simple"       },
    { .key = "imagine",       .value = "imagine"      },
    { .key = "lettuce",       .value = "lettuce"      },
    { .key = "responsible",   .value = "responsible"  },
    { .key = "certain",       .value = "certain"      },
    { .key = "tedious",       .value = "tedious"      },
    { .key = "earn",          .value = "earn"         },
    { .key = "zebra",         .value = "zebra"        },
    { .key = "teeny",         .value = "teeny"        },
    { .key = "pathetic",      .value = "pathetic"     },
    { .key = "license",       .value = "license"      },
    { .key = "clover",        .value = "clover"       },
    { .key = "jewel",         .value = "jewel"        },
    { .key = "jolly",         .value = "jolly"        },
    { .key = "tawdry",        .value = "tawdry"       },
    { .key = "harm",          .value = "harm"         },
    { .key = "fit",           .value = "fit"          },
    { .key = "satisfying",    .value = "satisfying"   },
    { .key = "breezy",        .value = "breezy"       },
    { .key = "nod",           .value = "nod"          },
    { .key = "power",         .value = "power"        },
    { .key = "dusty",         .value = "dusty"        },
    { .key = "birds",         .value = "birds"        },
    { .key = "rain",          .value = "rain"         },
    { .key = "handsome",      .value = "handsome"     },
    { .key = "worm",          .value = "worm"         },
    { .key = "potato",        .value = "potato"       },
    { .key = "nosy",          .value = "nosy"         },
    { .key = "spring",        .value = "spring"       },
    { .key = "neighborly",    .value = "neighborly"   },
    { .key = "work",          .value = "work"         },
    { .key = "hospitable",    .value = "hospitable"   },
    { .key = "sick",          .value = "sick"         },
    { .key = "subsequent",    .value = "subsequent"   },
    { .key = "tomatoes",      .value = "tomatoes"     },
    { .key = "blind",         .value = "blind"        },
    { .key = "silly",         .value = "silly"        },
    { .key = "scrawny",       .value = "scrawny"      },
    { .key = "lumpy",         .value = "lumpy"        },
    { .key = "salty",         .value = "salty"        },
    { .key = "bent",          .value = "bent"         },
    { .key = "cloth",         .value = "cloth"        },
    { .key = "many",          .value = "many"         },
    { .key = "furniture",     .value = "furniture"    },
    { .key = "plantation",    .value = "plantation"   },
    { .key = "fearful",       .value = "fearful"      },
    { .key = "nutritious",    .value = "nutritious"   },
    { .key = "bell",          .value = "bell"         },
    { .key = "motionless",    .value = "motionless"   },
    { .key = "friction",      .value = "friction"     },
    { .key = "melted",        .value = "melted"       },
    { .key = "answer",        .value = "answer"       },
    { .key = "waiting",       .value = "waiting"      },
    { .key = "curvy",         .value = "curvy"        },
    { .key = "basket",        .value = "basket"       },
    { .key = "flame",         .value = "flame"        },
    { .key = "sore",          .value = "sore"         },
    { .key = "imported",      .value = "imported"     },
    { .key = "yam",           .value = "yam"          },
    { .key = "thing",         .value = "thing"        },
    { .key = "jog",           .value = "jog"          },
    { .key = "panoramic",     .value = "panoramic"    },
    { .key = "blot",          .value = "blot"         },
    { .key = "mute",          .value = "mute"         },
    { .key = "flagrant",      .value = "flagrant"     },
    { .key = "big",           .value = "big"          },
    { .key = "name",          .value = "name"         },
    { .key = "quarrelsome",   .value = "quarrelsome"  },
    { .key = "type",          .value = "type"         },
    { .key = "heavenly",      .value = "heavenly"     },
    { .key = "cart",          .value = "cart"         },
    { .key = "underwear",     .value = "underwear"    },
    { .key = "fierce",        .value = "fierce"       },
    { .key = "car",           .value = "car"          },
    { .key = "precious",      .value = "precious"     },
    { .key = "jazzy",         .value = "jazzy"        },
    { .key = "acceptable",    .value = "acceptable"   },
    { .key = "secretary",     .value = "secretary"    },
    { .key = "radiate",       .value = "radiate"      },
    { .key = "rule",          .value = "rule"         },
    { .key = "wine",          .value = "wine"         },
    { .key = "sleet",         .value = "sleet"        },
    { .key = "steel",         .value = "steel"        },
    { .key = "measly",        .value = "measly"       },
    { .key = "reminiscent",   .value = "reminiscent"  },
    { .key = "straw",         .value = "straw"        },
    { .key = "pause",         .value = "pause"        },
    { .key = "staking",       .value = "staking"      },
    { .key = "feigned",       .value = "feigned"      },
    { .key = "multiply",      .value = "multiply"     },
    { .key = "trucks",        .value = "trucks"       },
    { .key = "stain",         .value = "stain"        },
    { .key = "sparkle",       .value = "sparkle"      },
    { .key = "numerous",      .value = "numerous"     },
    { .key = "pie",           .value = "pie"          },
    { .key = "calculating",   .value = "calculating"  },
    { .key = "unit",          .value = "unit"         },
    { .key = "history",       .value = "history"      },
    { .key = "satisfy",       .value = "satisfy"      },
    { .key = "excite",        .value = "excite"       },
    { .key = "cause",         .value = "cause"        },
    { .key = "better",        .value = "better"       },
    { .key = "scare",         .value = "scare"        },
    { .key = "front",         .value = "front"        },
    { .key = "puzzling",      .value = "puzzling"     },
    { .key = "discreet",      .value = "discreet"     },
    { .key = "attempt",       .value = "attempt"      },
    { .key = "signal",        .value = "signal"       },
    { .key = "arrive",        .value = "arrive"       },
    { .key = "shirt",         .value = "shirt"        },
    { .key = "racial",        .value = "racial"       },
    { .key = "ludicrous",     .value = "ludicrous"    },
    { .key = "frequent",      .value = "frequent"     },
    { .key = "miss",          .value = "miss"         },
    { .key = "waggish",       .value = "waggish"      },
    { .key = "lovely",        .value = "lovely"       },
    { .key = "four",          .value = "four"         },
    { .key = "channel",       .value = "channel"      },
    { .key = "stretch",       .value = "stretch"      },
    { .key = "wilderness",    .value = "wilderness"   },
    { .key = "gray",          .value = "gray"         },
    { .key = "fearless",      .value = "fearless"     },
    { .key = "grumpy",        .value = "grumpy"       },
    { .key = "adjustment",    .value = "adjustment"   },
    { .key = "coherent",      .value = "coherent"     },
    { .key = "part",          .value = "part"         },
    { .key = "burn",          .value = "burn"         },
    { .key = "observation",   .value = "observation"  },
    { .key = "haircut",       .value = "haircut"      },
    { .key = "gleaming",      .value = "gleaming"     },
    { .key = "useful",        .value = "useful"       },
    { .key = "substance",     .value = "substance"    },
    { .key = "beam",          .value = "beam"         },
    { .key = "zinc",          .value = "zinc"         },
    { .key = "geese",         .value = "geese"        },
    { .key = "smash",         .value = "smash"        },
    { .key = "ruin",          .value = "ruin"         },
    { .key = "guttural",      .value = "guttural"     },
    { .key = "milk",          .value = "milk"         },
    { .key = "stuff",         .value = "stuff"        },
    { .key = "bumpy",         .value = "bumpy"        },
    { .key = "gabby",         .value = "gabby"        },
    { .key = "instrument",    .value = "instrument"   },
    { .key = "business",      .value = "business"     },
    { .key = "rat",           .value = "rat"          },
    { .key = "houses",        .value = "houses"       },
    { .key = "dashing",       .value = "dashing"      },
    { .key = "fireman",       .value = "fireman"      },
    { .key = "lewd",          .value = "lewd"         },
    { .key = "fence",         .value = "fence"        },
    { .key = "sleep",         .value = "sleep"        },
    { .key = "dirty",         .value = "dirty"        },
    { .key = "room",          .value = "room"         },
    { .key = "program",       .value = "program"      },
    { .key = "detail",        .value = "detail"       },
    { .key = "pail",          .value = "pail"         },
    { .key = "summer",        .value = "summer"       },
    { .key = "holistic",      .value = "holistic"     },
    { .key = "adamant",       .value = "adamant"      },
    { .key = "odd",           .value = "odd"          },
    { .key = "spicy",         .value = "spicy"        },
};

void
print_hashmap(const struct hashmap* map)
{
    printf("\nHashmap has %zu buckets\n", map->buckets.length);
    for (size_t i = 0; i < map->buckets.length; i++) {
        struct list_iter it = list_iter(map->buckets.elements[i]);

        printf("  %zu =>\n", i);

        list_iter_next(&it);
        while (list_iter_next(&it)) {
            struct list*          item  = list_iter_get(&it);
            struct hashmap_entry* entry = item->data;

            if (entry != NULL) {
                printf("    - %s -> %s\n",
                       (const char*)entry->key,
                       (const char*)entry->value);
            }
            else {
                printf("Dead entry -> ");
            }
        }

        printf("\n");
    }
}

struct hashmap
make_hashmap(struct entry* entries, size_t n_entries)
{
    struct hashmap map;

    hashmap_init(&map, hash_str, compare_str);
    for (size_t i = 0; i < n_entries; i++) {
        hashmap_set(&map, entries[i].key, entries[i].value);
    }

    return map;
}

void
test_insertion(void)
{
    const size_t n_entries = sizeof(large_entries) / sizeof(large_entries[0]);
    struct hashmap map     = make_hashmap(large_entries, n_entries);

    for (size_t i = 0; i < n_entries; i++) {
        struct hashmap_entry* entry
            = hashmap_lookup(&map, large_entries[i].key);

        CU_ASSERT(entry != NULL);
        CU_ASSERT(strcmp(entry->key, large_entries[i].key) == 0);
        CU_ASSERT(strcmp(entry->key, entry->value) == 0);
        CU_ASSERT(entry->alive);
    }

    hashmap_destroy(&map);
}

void
test_remove(void)
{
    const size_t n_entries = 100;
    char* remove_keys[] = {
        "tender", "forgetful", "boring", "overt", "save", "wooden", "acid"
    };
    const size_t n_remove_keys = sizeof(remove_keys) / sizeof(remove_keys[0]);
    
    struct hashmap map = make_hashmap(large_entries, n_entries);

    /* Make sure the hashmap actually contains the entries first */
    for (size_t i = 0; i < n_remove_keys; i++) {
        struct hashmap_entry* e = hashmap_lookup(&map, remove_keys[i]);
        CU_ASSERT(e != NULL);
        CU_ASSERT(strcmp(e->key, remove_keys[i]) == 0);
    }

    for (size_t i = 0; i < n_remove_keys; i++) {
        hashmap_remove(&map, remove_keys[i]);
    }

    /* Ensure the entries were actually removed */
    for (size_t i = 0; i < n_remove_keys; i++) {
        struct hashmap_entry* e = hashmap_lookup(&map, remove_keys[i]);
        CU_ASSERT(e == NULL);
    }

    hashmap_destroy(&map);
}

void
test_iter(void)
{
    const size_t n_entries  = sizeof(large_entries) / sizeof(large_entries[0]);
    struct hashmap      map = make_hashmap(large_entries, n_entries);
    struct hashmap_iter it  = hashmap_iter(&map);

    /* Counts of number of entries for each iteration method (iter,
     * manual) */
    size_t count_iter = 0;
    size_t count_manual = 0;
    
    /* used to keep track of all hashmap entries we've visited */
    struct entry_visited iter_visited[n_entries];
    struct entry_visited manual_visited[n_entries];

    for (size_t i = 0; i < n_entries; i++) {
        iter_visited[i].visited = 0;
        iter_visited[i].key = large_entries[i].key;

        manual_visited[i].visited = 0;
        manual_visited[i].key = large_entries[i].key;
    }

    /* Check the size of the hashmap using an iterator */
    while (hashmap_iter_next(&it)) {
        struct hashmap_entry* e = hashmap_iter_get(&it);
        
        count_iter++;

        for (size_t v = 0; v < n_entries; v++) {
            if (strcmp(iter_visited[v].key, e->key) == 0) {
                CU_ASSERT(!iter_visited[v].visited);
                iter_visited[v].visited = 1;
                break;
            }
        }
    }

    /* Manually kludge our way through the hashmap */
    for (size_t i = 0; i < map.buckets.length; i++) {
        struct list* l = map.buckets.elements[i];
        l              = l->next;

        while (l) {
            struct hashmap_entry* e = l->data;
            
            for (size_t v = 0; v < n_entries; v++) {
                if (strcmp(manual_visited[v].key, e->key) == 0) {
                    CU_ASSERT(!manual_visited[v].visited);
                    manual_visited[v].visited = 1;
                    break;
                }
            }

            count_manual++;
            l = l->next;
        }
    }

    /* Make sure all our size measurements agree */
    CU_ASSERT(count_iter == n_entries);
    CU_ASSERT(count_manual == n_entries);
    CU_ASSERT(count_iter == count_manual);

    /* Make sure we actually visited each item (through both iterator
     * and manual methods) */
    for (size_t i = 0; i < n_entries; i++) {
        CU_ASSERT(iter_visited[i].visited);
        CU_ASSERT(manual_visited[i].visited);
    }

    hashmap_destroy(&map);
}

void
test_iter_after_remove(void)
{
    const size_t n_entries = sizeof(large_entries) / sizeof(large_entries[0]);
    char* remove_keys[] = {
        "tender", "forgetful", "boring", "overt", "save", "wooden", "acid"
    };

    const size_t n_remove_keys = sizeof(remove_keys) / sizeof(remove_keys[0]);
    
    struct hashmap map = make_hashmap(large_entries, n_entries);

    size_t count_iter = 0;
    size_t count_iter_after_remove = 0;

    struct hashmap_iter it1 = hashmap_iter(&map);
    while (hashmap_iter_next(&it1)) {
        count_iter++;
    }

    for (size_t i = 0; i < n_remove_keys; i++) {
        hashmap_remove(&map, remove_keys[i]);
    }

    struct hashmap_iter it2 = hashmap_iter(&map);
    while (hashmap_iter_next(&it2)) {
        struct hashmap_entry* e = hashmap_iter_get(&it2);
        count_iter_after_remove++;

        for (size_t i = 0; i < n_remove_keys; i++) {
            CU_ASSERT(strcmp(remove_keys[i], e->key) != 0);
        }
    }

    CU_ASSERT(count_iter - n_remove_keys == count_iter_after_remove);
}

static struct test_case tests[] = {
    { .name = "test hashmap insertion",    .test_function = test_insertion},
    { .name = "test hashmap remove", .test_function = test_remove },
    { .name = "test hashmap iterator", .test_function = test_iter },
    { .name = "test hashmap iterator after remove", .test_function = test_iter_after_remove },
};

TEST_MAIN("hashmaps", tests)
