package com.fant.util;

import java.util.List;

public class CompanyExtracter {
    public static void main(String[] args) {
        System.loadLibrary("search");
	  init("/home/fengxw/model", 2);
	  long startTime = System.currentTimeMillis();
	  System.out.println(search("北京奇格翱翔商贸有限公司北京凯达鼎盛建材有限公司北京东邦御厨厨房设备有限公司第二分公司北京天鸿恒达能源科技弿发有限公司", true));
        System.out.println("time: " + (System.currentTimeMillis() - startTime));
        destroy();
     }
    public static native List<String> search(String str, boolean greedy);

    public static native void init(String model_dir, int cacheLevel);

    public static native void destroy();
}
