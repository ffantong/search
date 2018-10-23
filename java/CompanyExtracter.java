package com.fant.util;

import java.util.List;

public class CompanyExtracter {
    public static void main(String[] args) {
        System.load("c:/Users/fengxw/Desktop/search/search/java/search.dll");
	  init("/home/fengxw/model", 2);
	  long startTime = System.currentTimeMillis();
	  System.out.println(search("啊啊啊啊", true));
        System.out.println("time: " + (System.currentTimeMillis() - startTime));
        destroy();
     }
    public static native List<String> search(String str, boolean greedy);

    public static native void init(String model_dir, int cacheLevel);

    public static native void destroy();
}
