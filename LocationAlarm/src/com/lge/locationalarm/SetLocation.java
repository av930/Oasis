package com.lge.locationalarm;

import java.util.ArrayList;
import java.util.List;
import java.util.Locale;

import com.google.android.maps.GeoPoint;
//import com.google.android.maps.GestureDetector.OnGestureListener;
import com.google.android.maps.ItemizedOverlay;
import com.google.android.maps.MapActivity;
import com.google.android.maps.MapController;
import com.google.android.maps.MapView;
import com.google.android.maps.Overlay;
import com.google.android.maps.OverlayItem;

import android.content.Context;

import android.graphics.drawable.Drawable;
import android.location.Address;
import android.location.Criteria;
import android.location.Geocoder;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.GestureDetector;
import android.view.GestureDetector.OnGestureListener;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

public class SetLocation extends MapActivity  {
    private final String LOG_TAG = "kh";
    MapView mMapView;
    Button mButton01;
    Button mButton02;
    private LocationManager locM; //위치 매니저
    private LocationListener locL; //위치 리스너
    private Location currentLocation; //현재 위치
    private String bestProvider; //현재 위치값을 가져오기위한 프로바이더. (network, gps)
    private List<Overlay> listOfOverlays; //맵에 표시된 오버레이(레이어)들을 가지고 있는 리스트
    private LocationItemizedOverlay overlayHere; //현재위치 마커가 표시되어질 오버레이
    private LocationItemizedOverlay overlayBranch; //지점위치 마커들이 표시되어질 오버레이
    private MapController mapController; //맵을 줌시키거나, 이동시키는데 사용될 컨트롤러



    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.setlocation_layout);
        mMapView = (MapView)findViewById(R.id.mapview);
        
        mMapView.setBuiltInZoomControls(true); //줌인,줌아웃 컨트롤을 표시한다.
        mapController = mMapView.getController(); //맵컨트롤러를 가져온다.
        mapController.setZoom(20); //초기 확대는 17정도로..
//        mMapView.setSatellite(true);
        //위치 매니저를 시스템으로부터 받아온다.
        locM = (LocationManager) getSystemService(Context.LOCATION_SERVICE);
        //사용가능한 적절한 프로바이더를 받아온다.
        //network (보통 3G망,Wifi AP 위치정보)또는 gps 둘중 하나로 설정된다.
        bestProvider = locM.getBestProvider(new Criteria(), true);
        //기기에 가지고 있는 마지막 위치정보로 현재위치를 초기 설정한다.
        currentLocation = locM.getLastKnownLocation(bestProvider);
        //위치 리스너 초기화
        locL = new MyLocationListener();
        //위치 매니저에 위치 리스너를 셋팅한다.
        //위치 리스너에서 10000ms (10초) 마다 100미터 이상 이동이 발견되면 업데이트를 하려한다.
        locM.requestLocationUpdates(bestProvider, 10000, 100, locL);
        //처음에 한번 맵뷰에 그려준다.
        updateOverlay(currentLocation);

        mButton01 	= (Button) findViewById(R.id.Button01);
        mButton01.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                MapController control = mMapView.getController();
                control.zoomIn();
            }
        });
        
        mButton02 	= (Button) findViewById(R.id.Button02);
        mButton02.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                MapController control = mMapView.getController();
                control.zoomOut();
            }
        });
    }

    @Override
    protected boolean isRouteDisplayed() {
        // TODO Auto-generated method stub
        return false;
    }

    public class MyLocationListener implements LocationListener {
        //@Override
        public void onLocationChanged(Location location) {
            //위치 이동이 발견되었을때 호출될 메소드.
            //위의 설정에서 10초마다 100미터 이상 이동이 발견되면 호출된다.
            updateOverlay(location);
        }
        //@Override
        public void onProviderDisabled(String provider) {
            Log.d(LOG_TAG, "GPS disabled : " + provider);
        }
        //@Override
        public void onProviderEnabled(String provider) {
            Log.d(LOG_TAG, "GPS Enabled : " + provider);
        }
        //@Override
        public void onStatusChanged(String provider, int status, Bundle extras) {
            Log.d(LOG_TAG, "onStatusChanged : " + provider + " & status = "
            + status);
        }
    }

    protected void updateOverlay(Location location) {

        //기존에 화면에 찍어둔 오버레이 (마커들)을 싹 지운다.
        listOfOverlays = mMapView.getOverlays(); //맵뷰에서 오버레이 리스트를 가져온다.
        if (listOfOverlays.size() > 0) {
            listOfOverlays.clear(); //오버레이가 있을때 싹 지워준다.
            Log.d(LOG_TAG, "clear overlays : " + listOfOverlays.size());
        } else {
        	Log.d(LOG_TAG, "empty overlays");
        }
        //Location 객체를 가지고 GeoPoint 객체를 얻어내는 메소드
        GeoPoint geoPoint = getGeoPoint(location);
        //현재위치를 표시할 이미지
        Drawable marker;
        //실제 운영소스엔 분기하여 현재위치와 선택위치 이미지를 변경하게 되어있다.
        marker = getResources().getDrawable(R.drawable.icon);
        marker.setBounds(0, 0, marker.getIntrinsicWidth(), marker.getIntrinsicHeight());
        //LocationItemizedOverlay 를 이용하여 현재위치 마커를 찍을 오버레이를 생성한다.
        overlayHere = new LocationItemizedOverlay(marker);
        //touch event 의 null pointer 버그를 방지하기 위해 마커를 찍고 바로 populate 시켜준다.
        overlayHere.mPopulate();
        //현재위치를 GeoCoder 를 이용하여 대략주소와 위,경도를 Toast 를 통하여 보여준다.
        String geoString;
        if(location == null)
        	geoString = "nowhere";
        else         	
        	geoString = showNowHere(location.getLatitude(), location.getLongitude() , true);
        //현재위치 마커 정의
        OverlayItem overlayItem = new OverlayItem(geoPoint, "here", geoString);
        overlayHere.addOverlay(overlayItem); //현재위치 오버레이 리스트에 현재위치 마커를 넣는다.
        // 지점정보를 HTTP통신을 통해 서버에서 받아와서 전역변수인 brList (지점리스트)에 넣는다.
        // 성능을 고려하여 쓰레드로 구현이 되어있다.
        // 고다음 지점리스트 오버레이에 넣고 화면에 찍어주는 메소드.
//        showBranchMarker(location.getLatitude(), location.getLongitude(),
//                         this.searchType, SEARCH_RANGE);
        // 맵뷰에서 터치이벤트를 받을 오버레이를 추가한다.
        // 특정지점을 오래 눌렀을때 특정 지점 기준으로 재검색을 하기 위하여 터치이벤트를 받아와야한다.
        mMapView.getOverlays().add(new MapTouchDetectorOverlay());
        // 마지막으로 생성된 오버레이레이어를 맵뷰에 추가한다.
        mMapView.getOverlays().add(overlayHere);
        mMapView.getController().animateTo(geoPoint); //현재위치로 화면을 이동한다.
        mMapView.postInvalidate(); //맵뷰를 다시 그려준다.

    }

    private String showNowHere(double lat, double lng , boolean showOption) {
        StringBuilder geoString = new StringBuilder();
        try {
            Geocoder goecoder = new Geocoder(getApplicationContext(),
                                             Locale.getDefault());
            Address adr = goecoder.getFromLocation(lat,
                                                   lng, 1).get(0);
            if (adr.getLocality() != null) geoString.append(adr.getLocality()).append(" ");
            if (adr.getThoroughfare() != null) geoString.append(adr.getThoroughfare());
            if (!"".equals(geoString.toString())) geoString.append("\n\n");
        } catch (Exception e) { }
        geoString.append("위도 : ").append(lat).append(" ,경도 : ").append(lng);
        if (showOption) {
            Toast.makeText(getApplicationContext(), geoString.toString(),
                           Toast.LENGTH_SHORT).show();
        }
        return geoString.toString();
    }


    private GeoPoint getGeoPoint(Location location) {
        if (location == null) {
            return null;
        }
        Double lat = location.getLatitude() * 1E6;
        Double lng = location.getLongitude() * 1E6;
        return new GeoPoint(lat.intValue(), lng.intValue());
    }


    protected class LocationItemizedOverlay extends
                ItemizedOverlay<OverlayItem> {
        private List<OverlayItem> overlays;
        public LocationItemizedOverlay(Drawable defaultMarker) { //오버레이 생성자
            //마커 이미지의 가운데 아랫부분이 마커에서 표시하는 포인트가 되게 한다.
            super(boundCenterBottom(defaultMarker));
            overlays = new ArrayList<OverlayItem>();
        }
        @Override
        protected OverlayItem createItem(int i) {
            return overlays.get(i);
        }
        @Override
        public int size() {
            return overlays.size();
        }
        public void addOverlay(OverlayItem overlay) {
            overlays.add(overlay);
            //null pointer 버그때문에 오버레이 아이템 추가후 가능한 빨리 populate 해줘야한다.
            populate();
        }
        @Override
        protected boolean onTap(int index) {
            //마커를 눌렀을때 발생시킬 이벤트 메소드이다.
            if ("here".equals(overlays.get(index).getTitle())) {
                //현재 위치일 경우 간단한 토스트 메세지를 보여준다.
                Toast.makeText(getApplicationContext(),
                               overlays.get(index).getSnippet(), Toast.LENGTH_SHORT)
                .show();
            } else {
                //지점선택일 경우 다이얼로그를 통하여 지점정보를 보여준다.
                //‘전화걸기’ 버튼으로 지점으로 전화거는 기능도 추가되어있다.
                //맵뷰에 관련한 소스가 아니어서 이곳에서는 표시 하지 않는다.
//	...
            }
            return true;
        }
        //외부에서 마커의 populate 를 해주기 위한 메소드.
        public void mPopulate() {
            populate();
        }
    }

//	지점 정보를 HTTP 통신을 통해 가져오는 메소드이다.
//	HTTP 통신시 랙현상을 없애기위해 쓰레드로 구현을 해봤다.
//	근데 스레드가 생각한대로 동작하진 않는것 같다. 잘못쓰고 있는것일까... -_-
    /*
    private void showBranchMarker(Double lat, Double lng, String searchType,
                                  String searchRange) {
        GetMapDataThread excuteThread = new GetMapDataThread(getMapdataHandler,
                lat, lng, searchType, searchRange);
        excuteThread.start();
    }
    */
    /*
    final Handler getMapdataHandler = new Handler() {
        public void handleMessage(Message msg) {
            if (msg.getData().getBoolean("SUCCESS_KEY")) {  // HTTP 통신이 성공적으로 이루어졌을때.
                // draw branches
                Drawable branchMarker;
                int markerType = 0;
                if ("0".equals(searchType)) { //검색조건에따라 마커이미지를 지점,ATM 중에 선택
                    markerType = R.drawable.icon_branch;
                } else if ("1".equals(searchType)) {
                    markerType = R.drawable.icon_atm;
                }
                branchMarker = getResources().getDrawable(markerType);
                branchMarker.setBounds(0, 0, branchMarker.getIntrinsicWidth(),
                                       branchMarker.getIntrinsicHeight());
                Double lat, lng;
                //지점 마커들을 그려줄 오버레이를 준비한다.
                overlayBranch = new LocationItemizedOverlay(branchMarker);
                overlayBranch.mPopulate();
                StringBuilder sb;
                //반복문을 돌면서 마커들을 오버레이에 추가한다.
                //나중에 마커를 눌렀을때 다이얼로그에 지점 정보를 보여주기위해 스니펫에 몇가지 정보를
                //string 으로 전달한다.
                              for (BranchInfoDTO d : brList) {
                                    lat = Double.parseDouble(d.getYCord()) * 1E6;
                                    lng = Double.parseDouble(d.getXCord()) * 1E6;
                                    GeoPoint branchGeoPoint = new GeoPoint(lat.intValue(),
                                                                           lng.intValue());
                                    sb = new StringBuilder();
                                    sb.append(d.getBussBrNm()).append(";")
                                    .append(d.getBussBrTelNo()).append(";")
                                    .append(d.getBussBrAdr()).append(";")
                                    .append(d.getTrscDrtm()).append(";")
                                    .append(d.getBussBrAdr2());
                                    // Create new overlay with marker at geoPoint
                                    OverlayItem overlayItem = new OverlayItem(branchGeoPoint,
                                            "branch", sb.toString());
                                    overlayBranch.addOverlay(overlayItem);
                                }
                
                //마커 찍은것이 없으면 오류 메세지를 토스트로 보여준다.
                if (overlayBranch.size() < 1) {
                    Toast.makeText(getApplicationContext(),
                                   "검색결과가 없거나 통신장애 입니다.\n'메뉴'버튼을 눌러 조건을 변경하여 다시 검색해 주세요.",
                                   Toast.LENGTH_LONG).show();
                }
                //지점 오버레이를 맵뷰 오버레이에 최종적으로 추가해준다.
                if (overlayBranch != null) {
                    mMapView.getOverlays().add(overlayBranch);
                    mMapView.postInvalidate();
                }
            }
        }
    };
    */
    /*
    private class GetMapDataThread extends Thread {
        private Handler tHandler;
        private Double lat, lng;
        private String searchType;
        private String searchRange;
        public GetMapDataThread(Handler tHandler) {
            this.tHandler = tHandler;
        }
        public GetMapDataThread(Handler tHandler, Double lat, Double lng,
                                String searhType, String searchRange) {
            this(tHandler); //스레드 처리 완료후 지도에 가져온 지점정보를 가지고 마커를 찍어줄 핸들러
            this.lat = lat; //위도
            this.lng = lng; //경도
            this.searchType = searhType; //검색조건 (0 : 지점, 1: ATM)
            this.searchRange = searchRange; //검색범위 단위는 m(미터)이다.
        }
        @Override
        public void run() { //스레드 실행~
            Bundle bundle = new Bundle();
            try {
                //전역변수로 선언한 지점 리스트를 준비한다. BranchInfoDTO 는 도메인이다.
                brList = new ArrayList<BranchInfoDTO>();
                brList = gdA.getMapData(lat.toString(), lng.toString(),
                                        searchType, searchRange);
                //gdA 클래스는 HTTP 통신을 해서 지점정보를 가져오는 클래스이다.
                //여기서는 설명하지 않았다. onCreate 에서 생성했다.
                bundle.putBoolean("SUCCESS_KEY", true); //성공하면 번들에 성공메세지 셋팅
            } catch (Exception e) {
//		...
                bundle.putBoolean("SUCCESS_KEY", false); //실패하면 false 이다.
                // ignore
            } finally {
                try {
                    Message msg = tHandler.obtainMessage();
                    msg.setData(bundle);
                    tHandler.sendMessage(msg); //핸들러에 메세지를 보낸다.
                    interrupt();
                } catch (Exception e) {
                    // ignore
                }
            }
        }
    }
*/
    
    public class MapTouchDetectorOverlay extends Overlay implements
                OnGestureListener {
        private GestureDetector gestureDetector;
        //onTouchEvent 의 ACTION_DOWN 등을 가지고 직접 처리 하지 않고
        //제스처들을 쉽게 캐치할수있는 리스너이다.
        private OnGestureListener onGestureListener;
        private static final long LOOOOONG_PRESS_MILLI_SEC = 1500; // 1.5초정도를 길게누름으로 인식한다.
        // for touch timer
        private Handler mHandler;
        private long touchStartTime;
        private long longPressTime;
        private MotionEvent globalEvent;
        //생성자
        public MapTouchDetectorOverlay() {
            gestureDetector = new GestureDetector(this);
            init();
        }
        public MapTouchDetectorOverlay(OnGestureListener onGestureListener) {
            this();
            setOnGestureListener(onGestureListener);
            init();
        }
        
        //생성자들이 호출할 초기화 함수
        private void init() {
            mHandler = new Handler();
            globalEvent = null;
        }
        //길게누름을 감지할 스레드
        private Runnable looongPressDetector = new Runnable() {
            public void run() {
                //화면을 누르고 있던 시간
                long touchHoldTime = longPressTime - touchStartTime;
                if ((globalEvent != null)
                && (touchHoldTime > (LOOOOONG_PRESS_MILLI_SEC - 200))) { //조건중에 200ms 를 빼고 검사하는것은 기기마다 성능이 달라서 약간의 여유를 준것이다.
                    Log.d(LOG_TAG, "loooooong press detected!");
                    float x = globalEvent.getX();
                    float y = globalEvent.getY(); //화면에서 눌려있던 지점을 받아온다.
                    GeoPoint p = mMapView.getProjection().fromPixels((int) x,
                                 (int) y); //눌려있던 지점을 위도 경도로 바꿔준다.
                    Location selectedLocation = new Location(currentLocation);
                    selectedLocation.setLatitude((p.getLatitudeE6() / 1E6));
                    selectedLocation.setLongitude((p.getLongitudeE6() / 1E6));
                    currentLocation = selectedLocation;
                    locM.removeUpdates(locL); //현재위치 리스너를 잠시 없애버린다.
                    updateOverlay(currentLocation); //지점 재검색 및 마커 다시 표시
                    showNowHere((p.getLatitudeE6() / 1E6) , (p.getLongitudeE6() / 1E6) , true);
                }
            }
        };
        @Override
    	public boolean onTouchEvent(MotionEvent event, MapView mapView) {
        	if ( event.getAction() == MotionEvent.ACTION_DOWN ) {
//        		Toast.makeText(getBaseContext(), "HI", Toast.LENGTH_LONG).show();
        	}
//    		Toast.makeText(getBaseContext(), "aa", Toast.LENGTH_LONG).show();
        	if (gestureDetector.onTouchEvent(event)) {
        		return true;
        	}
        	onLongPress(event);
    		return false;
    	}
		//@Override
		public boolean onDown(MotionEvent e) {
			if (onGestureListener != null) {
				return onGestureListener.onDown(e);
			} else {
				// start timer
				touchStartTime = System.currentTimeMillis();
				mHandler.postDelayed(looongPressDetector,
				LOOOOONG_PRESS_MILLI_SEC);
				//1.5초 있다가 길게누름을 체크해본다.
			}
			return false;
		}
		//@Override
		public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX,
				float velocityY) {
			if (onGestureListener != null) {
				return onGestureListener.onFling(e1, e2, velocityX, velocityY);
			}
			return false;
		}
		//@Override
		public void onLongPress(MotionEvent e) {
			// TODO Auto-generated method stub
			if (onGestureListener != null) {
				onGestureListener.onLongPress(e);
			}
			//화면을 누르고 있으면 onLongPress 가 호출되는데 호출될때마다 체크할 시간을 변수에 넣는다.
			//이부분이 퍼포먼스 하락에 영향을 줄 것 같다.
			globalEvent = e;
			longPressTime = System.currentTimeMillis();
			Toast.makeText(getBaseContext(), "long press", Toast.LENGTH_LONG).show();			
		}
		//@Override
		public boolean onScroll(MotionEvent e1, MotionEvent e2,
				float distanceX, float distanceY) {
			// TODO Auto-generated method stub
			return false;
		}
		//@Override
		public void onShowPress(MotionEvent e) {
			// TODO Auto-generated method stub
			if (onGestureListener != null) {
				onGestureListener.onShowPress(e);
			}			
		}
		//@Override
		public boolean onSingleTapUp(MotionEvent e) {
			if (onGestureListener != null) {
				onGestureListener.onSingleTapUp(e);
			}
			return false;
		}
        public void setOnGestureListener(OnGestureListener onGestureListener) {
        	this.onGestureListener = onGestureListener;
        }		
    }
}