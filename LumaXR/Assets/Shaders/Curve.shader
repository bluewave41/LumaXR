Shader "Unlit/Curve"
{
    Properties
    {
        _MainTex ("Texture", 2D) = "white" {}
        _Radius ("Curve Radius", Float) = 1.6
        _WrapAngle("Wrap Angle (deg)", Float) = 120
    }
    SubShader
    {
        Tags { "RenderType"="Opaque" }
        LOD 100

        Pass
        {
            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag
            // make fog work
            #pragma multi_compile_fog

            #include "UnityCG.cginc"

            struct appdata
            {
                float4 vertex : POSITION;
                float2 uv : TEXCOORD0;
            };

            struct v2f
            {
                float2 uv : TEXCOORD0;
                UNITY_FOG_COORDS(1)
                float4 vertex : SV_POSITION;
            };

            sampler2D _MainTex;
            float4 _MainTex_ST;
            float _Radius;
            float _WrapAngle;

            v2f vert (appdata v)
            {
                v2f o;

                float width = 1.0;
                float t = (v.vertex.x + width * 0.5) / width;
                float angleRad = _WrapAngle * 3.14159265 / 180.0;
                float angle = -angleRad * 0.5 + t * angleRad;
                float x = sin(angle) * _Radius;
                float z = cos(angle) * _Radius;
                float y = v.vertex.y;
                float4 curvedVertex = float4(x, y, z, 1.0);
                o.vertex = UnityObjectToClipPos(curvedVertex);
                o.uv = TRANSFORM_TEX(v.uv, _MainTex);
                UNITY_TRANSFER_FOG(o,o.vertex);
                return o;
            }

            fixed4 frag (v2f i) : SV_Target
            {
                // sample the texture
                fixed4 col = tex2D(_MainTex, i.uv);
                // apply fog
                UNITY_APPLY_FOG(i.fogCoord, col);
                return col;
            }
            ENDCG
        }
    }
}
